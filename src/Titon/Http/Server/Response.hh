<?hh // partial
// Because of PSR HTTP Message
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http\Server;

use Psr\Http\Message\StreamableInterface;
use Titon\Common\FactoryAware;
use Titon\Http\Cookie;
use Titon\Http\Message;
use Titon\Http\Http;
use Titon\Http\Mime;
use Titon\Http\OutgoingResponse;
use Titon\Http\IncomingRequest;
use Titon\Http\IncomingRequestAware;
use Titon\Utility\Config;
use Titon\Utility\Format;
use Titon\Utility\Number;
use Titon\Utility\Str;
use Titon\Utility\Time;

/**
 * The Response object handles the collection and output of data to the browser. It stores a list of HTTP headers,
 * the content body, the content type and an associated status code to print out.
 *
 * @package Titon\Http\Server
 */
class Response extends Message implements OutgoingResponse {
    use FactoryAware, IncomingRequestAware;

    /**
     * Will return the response as a string instead of sending output.
     *
     * @var bool
     */
    protected bool $_debug = false;

    /**
     * Will add a Content-MD5 header based on the body.
     *
     * @var bool
     */
    protected bool $_md5 = false;

    /**
     * The HTTP protocol version.
     *
     * @var string
     */
    protected string $_protocolVersion = '1.1';

    /**
     * HTTP status code to output.
     *
     * @var int
     */
    protected int $_status = Http::OK;

    /**
     * Set body and status during initialization.
     *
     * @param \Psr\Http\Message\StreamableInterface $body
     * @param int $status
     */
    public function __construct(?StreamableInterface $body = null, int $status = Http::OK) {
        parent::__construct();

        $this
            ->date(time())
            ->connection(true)
            ->contentType('text/html')
            ->setStatus($status);

        if ($body) {
            $this->setBody($body);
        }
    }

    /**
     * Set the Accept-Ranges header.
     *
     * @param string $range
     * @return $this
     */
    public function acceptRanges(string $range = 'bytes'): this {
        return $this->setHeader('Accept-Ranges', $range);
    }

    /**
     * {@inheritdoc}
     */
    public function addHeader($key, $value): this {
        $this->headers->set($key, $value, true);

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function addHeaders(array $headers): this {
        foreach ($headers as $key => $value) {
            $this->addHeader($key, $value);
        }

        return $this;
    }

    /**
     * Set the Age header.
     *
     * @uses Titon\Utility\Time
     *
     * @param string|int $time
     * @return $this
     */
    public function age(mixed $time): this {
        if (is_string($time)) {
            $time = Time::toUnix($time) - time();
        }

        return $this->setHeader('Age', $time);
    }

    /**
     * Set the Allow header.
     *
     * @param Vector<string> $methods
     * @return $this
     */
    public function allow(Vector<string> $methods): this {
        return $this->setHeader('Allow', array_intersect(array_map(fun('strtoupper'), $methods), Http::getMethodTypes()));
    }

    /**
     * Alias for setBody().
     *
     * @param \Psr\Http\Message\StreamableInterface $body
     * @return $this
     */
    public function body(StreamableInterface $body): this {
        return $this->setBody($body);
    }

    /**
     * Force the clients browser to cache the current request.
     *
     * @param string $directive
     * @param int|string $expires
     * @param bool $proxy
     * @param Map<string, mixed> $options
     * @return $this
     * @throws \InvalidArgumentException
     */
    public function cache(string $directive, mixed $expires = '+24 hours', bool $proxy = true, Map<string, mixed> $options = Map {}): this {
        $expires = Time::toUnix($expires);

        if ($directive === 'none') {
            $control = Map {
                'no-cache' => true,
                'no-store' => true,
                'must-revalidate' => true
            };

            if ($proxy) {
                $control['proxy-revalidate'] = true;
            }

            $this->setHeader('Pragma', 'no-cache');

        } else if ($directive === 'public' || $directive === 'private') {
            $control = Map {$directive => true};
            $ttl = $expires - time();

            if ($ttl > 0) {
                $control['max-age'] = $ttl;

                if ($proxy) {
                    $control['s-maxage'] = $ttl;
                }
            }
        } else {
            throw new \InvalidArgumentException(sprintf('Invalid cache directive %s', $directive));
        }

        return $this->expires($expires)->cacheControl($control->setAll($options));
    }

    /**
     * Set the Cache-Control header.
     *
     * @uses Titon\Utility\Time
     *
     * @param Map<string, mixed> $values
     * @return $this
     */
    public function cacheControl(Map<string, mixed> $values): this {
        $header = [];

        foreach ($values as $key => $value) {
            if ($value === true) {
                $header[] = $key;
            } else {
                $value = (string) $value;

                if ($key === 'private' || $key === 'no-cache') {
                    $value = '"' . $value . '"';
                }

                $header[] = $key . '=' . $value;
            }
        }

        return $this->setHeader('Cache-Control', $header);
    }

    /**
     * Set the Connection header.
     *
     * @param string|bool $status
     * @return $this
     */
    public function connection(mixed $status): this {
        if ($status === true) {
            $status = 'keep-alive';

        } else if ($status === false) {
            $status = 'close';
        }

        return $this->setHeader('Connection', $status);
    }

    /**
     * Set the Content-Disposition header.
     *
     * @param string $file
     * @param string $type
     * @return $this
     * @throws \InvalidArgumentException
     */
    public function contentDisposition(string $file, string $type = 'attachment'): this {
        if ($type !== 'attachment' && $type !== 'inline') {
            throw new \InvalidArgumentException('Disposition type must be either "attachment" or "inline"');
        }

        return $this->setHeader('Content-Disposition', sprintf('%s; filename="%s"', $type, $file));
    }

    /**
     * Set the Content-Encoding header.
     *
     * @param string $encoding
     * @return $this
     */
    public function contentEncoding(string $encoding): this {
        return $this->setHeader('Content-Encoding', $encoding);
    }

    /**
     * Set the Content-Language header. Attempt to use the locales set in G11n.
     *
     * @param string $locales
     * @return $this
     */
    public function contentLanguage(string $locales): this {
        return $this->setHeader('Content-Language', $locales);
    }

    /**
     * Set the Content-Location header.
     *
     * @param string $location
     * @return $this
     */
    public function contentLocation(string $location): this {
        return $this->setHeader('Content-Location', $location);
    }

    /**
     * Set the Content-Length header.
     *
     * @uses Titon\Utility\Number
     *
     * @param string|int $length
     * @return $this
     */
    public function contentLength(mixed $length): this {
        if ($this->_status >= 300 && $this->_status <= 399) {
            return $this;
        }

        if (!is_numeric($length)) {
            $length = Number::bytesFrom($length);
        }

        return $this->setHeader('Content-Length', $length);
    }

    /**
     * Set the Content-MD5 header.
     *
     * @param bool|string $content
     * @return $this
     */
    public function contentMD5(mixed $content): this {
        if (is_bool($content)) {
            $this->_md5 = $content;
        } else {
            $this->setHeader('Content-MD5', $content);
        }

        return $this;
    }

    /**
     * Set the Content-Range header.
     *
     * @param int $start
     * @param int $end
     * @param int $size
     * @return $this
     */
    public function contentRange(int $start, int $end, int $size): this {
        return $this->setHeader('Content-Range', sprintf('bytes %s-%s/%s', $start, $end, $size));
    }

    /**
     * Set the Content-Type header. If the type is text based, include the app charset.
     *
     * @uses Titon\Utility\Config
     * @uses Titon\Http\Mime
     * @uses Titon\Utility\Str
     *
     * @param string $type
     * @return $this
     */
    public function contentType(string $type): this {
        if (in_array($this->_status, [Http::NOT_MODIFIED, Http::NO_CONTENT])) {
            return $this;
        }

        if (strpos($type, '/') === false) {
            $type = Mime::getTypeByExt($type);
        }

        $charset = Config::encoding();

        if ($charset && (Str::startsWith($type, Mime::TEXT) ||
            in_array($type, ['application/javascript', 'application/json', 'application/xml', 'application/rss+xml']))) {
            $type .= '; charset=' . $charset;
        }

        return $this->setHeader('Content-Type', $type);
    }

    /**
     * Set the Date header.
     *
     * @uses Titon\Utility\Format
     *
     * @param string|int $time
     * @return $this
     */
    public function date(mixed $time): this {
        return $this->setHeader('Date', Format::http($time));
    }

    /**
     * Enable debugging.
     *
     * @return $this
     */
    public function debug(): this {
        $this->_debug = true;

        return $this;
    }

    /**
     * Load a file and force a download using HTTP headers.
     *
     * @param string $path
     * @param string $name
     * @param bool $autoEtag
     * @param bool $autoModified
     * @return \Titon\Http\Server\DownloadResponse
     */
    public static function download(string $path, string $name = '', bool $autoEtag = false, bool $autoModified = true): DownloadResponse {
        $download = new DownloadResponse($path, Http::OK);

        if ($name) {
            $download->contentDisposition($name);
        }

        if ($autoEtag) {
            $download->etag(sha1_file($path));
        }

        if ($autoModified) {
            $download->lastModified(filemtime($path));
        }

        return $download;
    }

    /**
     * Set the ETag header. If the tag is null, generate a hash based off the current URL.
     *
     * @param string $tag
     * @param bool $weak
     * @return $this
     */
    public function etag(string $tag, bool $weak = false): this {
        return $this->setHeader('ETag', sprintf('%s"%s"', ($weak ? 'W/' : ''), $tag));
    }

    /**
     * Set the Expires header.
     *
     * @uses Titon\Utility\Format
     *
     * @param string|int $expires
     * @return $this
     */
    public function expires(mixed $expires = '+24 hours'): this {
        return $this->setHeader('Expires', Format::http($expires));
    }

    /**
     * {@inheritdoc}
     */
    public function getProtocolVersion(): string {
        return $this->_protocolVersion;
    }

    /**
     * {@inheritdoc}
     */
    public function getReasonPhrase(): string {
        return $this->getHeader('Reason-Phrase') ?: Http::getStatusCode($this->getStatusCode());
    }

    /**
     * {@inheritdoc}
     */
    public function getStatusCode(): int {
        return $this->_status;
    }

    /**
     * Return true if we are debugging.
     *
     * @return bool
     */
    public function isDebugging(): bool {
        return $this->_debug;
    }

    /**
     * Convert a resource to JSON by instantiating a JsonResponse.
     * Can optionally pass encoding options, and a JSONP callback.
     *
     * @param mixed $data
     * @param int $flags
     * @param string $callback
     * @return \Titon\Http\Server\JsonResponse
     */
    public static function json(mixed $data, int $flags = 0, string $callback = ''): JsonResponse {
        return new JsonResponse($data, Http::OK, $flags, $callback);
    }

    /**
     * Set the Last-Modified header.
     *
     * @uses Titon\Utility\Format
     *
     * @param string|int $time
     * @return $this
     */
    public function lastModified(mixed $time = null): this {
        return $this->setHeader('Last-Modified', Format::http($time));
    }

    /**
     * Set the Location header.
     *
     * @param string $url
     * @return $this
     */
    public function location(string $url): this {
        return $this->setHeader('Location', $url);
    }

    /**
     * Forces the clients browser not to cache the results of the current request.
     *
     * @return $this
     */
    public function noCache(): this {
        return $this->cache('none', '-1 year')->lastModified();
    }

    /**
     * Set a response as 304 Not Modified and remove conflicting headers.
     *
     * @return $this
     */
    public function notModified(): this {
        $this->statusCode(Http::NOT_MODIFIED)->removeHeaders([
            'Allow',
            'Content-Disposition',
            'Content-Encoding',
            'Content-Language',
            'Content-Length',
            'Content-MD5',
            'Content-Type',
            'Last-Modified'
        ]);

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function prepare(IncomingRequest $request): this {
        $this->setRequest($request);

        return $this;
    }

    /**
     * Redirect to another URL by instantiating a new RedirectResponse.
     *
     * @param string $url
     * @param int $status
     * @return \Titon\Http\Server\RedirectResponse
     */
    public static function redirect(string $url, int $status = Http::FOUND): RedirectResponse {
        return new RedirectResponse($url, $status);
    }

    /**
     * Remove a cookie by setting a Set-Cookie header with a negative expires.
     *
     * @param string $name
     * @param string $path
     * @param string $domain
     * @param bool $httpOnly
     * @param bool $secure
     * @return $this
     */
    public function removeCookie(string $name, string $path = '/', string $domain = '', bool $httpOnly = true, bool $secure = false): this {
        return $this->setCookie($name, '', time(), $path, $domain, $httpOnly, $secure);
    }

    /**
     * {@inheritdoc}
     */
    public function removeHeader($key): this {
        $this->headers->remove($key);

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function removeHeaders(array $keys): this {
        foreach ($keys as $key) {
            $this->removeHeader($key);
        }

        return $this;
    }

    /**
     * Set the Retry-After header.
     *
     * @uses Titon\Utility\Format
     *
     * @param string|int $length
     * @return $this
     */
    public function retryAfter(mixed $length): this {
        if (is_string($length)) {
            $length = Format::http($length);
        }

        return $this->setHeader('Retry-After', $length);
    }

    /**
     * {@inheritdoc}
     */
    public function send(): string {
        $body = $this->getBody();
        $contents = (string) $body?->getContents();

        // Create an MD5 digest?
        if ($contents && $this->_md5) {
            $this->setHeader('Content-MD5', base64_encode(pack('H*', md5($contents))));
        }

        // Return while in debug
        if ($this->isDebugging()) {
            return $contents;
        }

        $this->sendHeaders();
        $this->sendBody();

        if (function_exists('fastcgi_finish_request')) {
            fastcgi_finish_request();
        }

        return $contents;
    }

    /**
     * Output the body by echoing its contents.
     * If a buffer is set, chunk the body into parts.
     *
     * @return $this
     */
    public function sendBody(): this {
        $body = $this->getBody();

        if ($body) {
            $chunks = str_split($body->getContents(), 8192);

            foreach ($chunks as $chunk) {
                echo $chunk;
            }
        }

        return $this;
    }

    /**
     * Output headers and cookies defined for the current response.
     *
     * @return $this
     */
    public function sendHeaders(): this {
        if (headers_sent()) {
            return $this;
        }

        $status = $this->getStatusCode();

        header(sprintf('HTTP/%s %s %s', $this->getProtocolVersion(), $status, $this->getReasonPhrase()), true, $status);

        foreach ($this->getHeaders() as $header => $values) {
            foreach ($values as $value) {
                header($header . ': ' . $value, false, $status);
            }
        }

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function setBody(StreamableInterface $body): this {
        $this->_body = $body;

        return $this;
    }

    /**
     * Set a cookie with the Set-Cookie header.
     *
     * @param string $name
     * @param string $value
     * @param mixed $expires
     * @param string $path
     * @param string $domain
     * @param bool $httpOnly
     * @param bool $secure
     * @return $this
     */
    public function setCookie(string $name, string $value, mixed $expires = 0, string $path = '/', string $domain = '', bool $httpOnly = true, bool $secure = false): this {
        return $this->addHeader('Set-Cookie', (string) new Cookie($name, $value, $expires, $path, $domain, $httpOnly, $secure));
    }

    /**
     * {@inheritdoc}
     */
    public function setHeader($key, $value): this {
        $this->headers->set($key, $value);

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function setHeaders(array $headers): this {
        foreach ($headers as $key => $value) {
            $this->setHeader($key, $value);
        }

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function setProtocolVersion($version): this {
        $this->_protocolVersion = $version;

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function setReasonPhrase($phrase): this {
        return $this->setHeader('Reason-Phrase', $phrase);
    }

    /**
     * {@inheritdoc}
     */
    public function setStatus($code, $reasonPhrase = null): this {
        if (Http::getStatusCode($code)) {
            $this->_status = $code;
        }

        return $this->setHeader('Status-Code', $code . ' ' . ($reasonPhrase ?: $this->getReasonPhrase()));
    }

    /**
     * Alias for setStatus().
     *
     * @param int $code
     * @return $this
     */
    public function statusCode(int $code): this {
        return $this->setStatus($code);
    }

    /**
     * Set the Vary header.
     *
     * @param string $variances
     * @return $this
     */
    public function vary(string $variances): this {
        return $this->setHeader('Vary', $variances);
    }

    /**
     * Set the WWW-Authenticate header.
     *
     * @param string $scheme
     * @return $this
     */
    public function wwwAuthenticate(string $scheme): this {
        return $this->setHeader('WWW-Authenticate', $scheme);
    }

    /**
     * Convert a resource to XML by instantiating a XmlResponse.
     *
     * @param mixed $data
     * @param string $root
     * @return \Titon\Http\Server\XmlResponse
     */
    public static function xml(mixed $data, string $root = 'root'): XmlResponse {
        return new XmlResponse($data, Http::OK, $root);
    }

}
