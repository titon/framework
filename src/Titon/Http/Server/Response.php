<?php
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http\Server;

use Titon\Common\Attachable;
use Titon\Common\FactoryAware;
use Titon\Http\AbstractMessage;
use Titon\Http\Bag\CookieBag;
use Titon\Http\Http;
use Titon\Http\Mime;
use Titon\Http\Request as BaseRequest;
use Titon\Http\Response as BaseResponse;
use Titon\Http\RequestAware;
use Titon\Utility\Config;
use Titon\Utility\Format;
use Titon\Utility\Number;
use Titon\Utility\String;
use Titon\Utility\Time;

/**
 * The Response object handles the collection and output of data to the browser. It stores a list of HTTP headers,
 * the content body, the content type and an associated status code to print out.
 *
 * @package Titon\Http\Server
 */
class Response extends AbstractMessage implements BaseResponse {
    use FactoryAware, RequestAware;

    /**
     * COOKIE data to set.
     *
     * @type \Titon\Http\Bag\CookieBag
     */
    public $cookies;

    /**
     * Configuration.
     *
     * @type array {
     *      @type int $buffer   The range in which to break up the body into chunks.
     *      @type bool $md5     When enabled, will add a Content-MD5 header based on the body.
     *      @type bool $debug   When enabled, will return the response as a string instead of outputting.
     * }
     */
    protected $_config = [
        'buffer' => 8192,
        'md5' => false,
        'debug' => false
    ];

    /**
     * HTTP status code to output.
     *
     * @type int
     */
    protected $_status = Http::OK;

    /**
     * Set body and status during initialization.
     *
     * @param string $body
     * @param int $status
     * @param array $config
     */
    public function __construct($body = '', $status = Http::OK, array $config = []) {
        parent::__construct($config);

        $this
            ->date(time())
            ->connection(true)
            ->contentType('text/html')
            ->setStatusCode($status)
            ->setBody($body);

        $this->cookies = new CookieBag();
    }

    /**
     * Set the Accept-Ranges header.
     *
     * @param string $range
     * @return $this
     */
    public function acceptRanges($range = 'bytes') {
        return $this->setHeader('Accept-Ranges', $range);
    }

    /**
     * Set the Age header.
     *
     * @uses Titon\Utility\Time
     *
     * @param string|int $time
     * @return $this
     */
    public function age($time) {
        if (is_string($time)) {
            $time = Time::toUnix($time) - time();
        }

        return $this->setHeader('Age', $time);
    }

    /**
     * Set the Allow header.
     *
     * @param string|array $methods
     * @return $this
     */
    public function allow($methods) {
        return $this->setHeader('Allow', array_intersect(array_map('strtoupper', (array) $methods), Http::getMethodTypes()));
    }

    /**
     * Alias for setBody().
     *
     * @param string $body
     * @return $this
     */
    public function body($body = null) {
        return $this->setBody($body);
    }

    /**
     * Force the clients browser to cache the current request.
     *
     * @param string $directive
     * @param int|string $expires
     * @param bool $proxy
     * @param array $options
     * @return $this
     * @throws \InvalidArgumentException
     */
    public function cache($directive, $expires = '+24 hours', $proxy = true, array $options = []) {
        $expires = Time::toUnix($expires);

        if ($directive === 'none') {
            $control = ['no-cache', 'no-store', 'must-revalidate'];

            if ($proxy) {
                $control[] = 'proxy-revalidate';
            }

            $this->setHeader('Pragma', 'no-cache');

        } else if ($directive === 'public' || $directive === 'private') {
            $control = [$directive];
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

        return $this->expires($expires)->cacheControl(array_merge($options, $control));
    }

    /**
     * Set the Cache-Control header.
     *
     * @uses Titon\Utility\Time
     *
     * @param array $values
     * @return $this
     */
    public function cacheControl($values) {
        $header = [];

        foreach ((array) $values as $key => $value) {
            if (is_numeric($key)) {
                $header[] = $value;
            } else {
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
    public function connection($status) {
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
    public function contentDisposition($file, $type = 'attachment') {
        if ($type !== 'attachment' && $type !== 'inline') {
            throw new \InvalidArgumentException('Disposition type must be either "attachment" or "inline"');
        }

        return $this->setHeader('Content-Disposition', sprintf('attachment; filename="%s"', $file));
    }

    /**
     * Set the Content-Encoding header.
     *
     * @param string|array $encoding
     * @return $this
     */
    public function contentEncoding($encoding) {
        return $this->setHeader('Content-Encoding', $encoding);
    }

    /**
     * Set the Content-Language header. Attempt to use the locales set in G11n.
     *
     * @param string|array $locales
     * @return $this
     */
    public function contentLanguage($locales = []) {
        return $this->setHeader('Content-Language', array_unique((array) $locales));
    }

    /**
     * Set the Content-Location header.
     *
     * @param string $location
     * @return $this
     */
    public function contentLocation($location) {
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
    public function contentLength($length) {
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
    public function contentMD5($content) {
        if (is_bool($content)) {
            $this->setConfig('md5', $content);
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
    public function contentRange($start, $end, $size) {
        return $this->setHeader('Content-Range', sprintf('bytes %s-%s/%s', $start, $end, $size));
    }

    /**
     * Set the Content-Type header. If the type is text based, include the app charset.
     *
     * @uses Titon\Utility\Config
     * @uses Titon\Http\Mime
     * @uses Titon\Utility\String
     *
     * @param string $type
     * @return $this
     */
    public function contentType($type) {
        if (in_array($this->_status, [Http::NOT_MODIFIED, Http::NO_CONTENT])) {
            return $this;
        }

        if (strpos($type, '/') === false) {
            $type = Mime::getTypeByExt($type);
        }

        $charset = Config::encoding();

        if ($charset && (String::startsWith($type, Mime::TEXT) ||
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
    public function date($time) {
        return $this->setHeader('Date', Format::http($time));
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
    public static function download($path, $name = null, $autoEtag = false, $autoModified = true) {
        return new DownloadResponse($path, Http::OK, [
            'autoEtag' => $autoEtag,
            'autoModified' => $autoModified,
            'dispositionName' => $name
        ]);
    }

    /**
     * Set the ETag header. If the tag is null, generate a hash based off the current URL.
     *
     * @param string $tag
     * @param bool $weak
     * @return $this
     */
    public function etag($tag, $weak = false) {
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
    public function expires($expires = '+24 hours') {
        return $this->setHeader('Expires', Format::http($expires));
    }

    /**
     * {@inheritdoc}
     */
    public function getProtocolVersion() {
        if ($request = $this->getRequest()) {
            return $request->getProtocolVersion();
        }

        return '1.1';
    }

    /**
     * {@inheritdoc}
     */
    public function getReasonPhrase() {
        return Http::getStatusCode($this->_status);
    }

    /**
     * {@inheritdoc}
     */
    public function getStatusCode() {
        return $this->_status;
    }

    /**
     * Convert a resource to JSON by instantiating a JsonResponse.
     * Can optionally pass encoding options, and a JSONP callback.
     *
     * @param mixed $data
     * @param int $options
     * @param string $callback
     * @return \Titon\Http\Server\JsonResponse
     */
    public static function json($data, $options = null, $callback = null) {
        return new JsonResponse($data, Http::OK, $options, $callback);
    }

    /**
     * Set the Last-Modified header.
     *
     * @uses Titon\Utility\Format
     *
     * @param string|int $time
     * @return $this
     */
    public function lastModified($time = null) {
        return $this->setHeader('Last-Modified', Format::http($time));
    }

    /**
     * Set the Location header.
     *
     * @param string $url
     * @return $this
     */
    public function location($url) {
        return $this->setHeader('Location', (string) $url);
    }

    /**
     * Forces the clients browser not to cache the results of the current request.
     *
     * @return $this
     */
    public function noCache() {
        return $this->cache('none', '-1 year')->lastModified();
    }

    /**
     * Set a response as 304 Not Modified and remove conflicting headers.
     *
     * @return $this
     */
    public function notModified() {
        $this->statusCode(Http::NOT_MODIFIED)->setBody(null);

        foreach ([
            'Allow',
            'Content-Disposition',
            'Content-Encoding',
            'Content-Language',
            'Content-Length',
            'Content-MD5',
            'Content-Type',
            'Last-Modified'
        ] as $header) {
            $this->headers->remove($header);
        }

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function prepare(BaseRequest $request) {
        $this->setRequest($request);

        return $this;
    }

    /**
     * Redirect to another URL by instantiating a new RedirectResponse.
     *
     * @param array|string $url
     * @param int $code
     * @return \Titon\Http\Server\RedirectResponse
     */
    public static function redirect($url, $code = Http::FOUND) {
        return new RedirectResponse($url, $code);
    }

    /**
     * Remove a cookie by setting a Set-Cookie header with a negative expires.
     *
     * @param string $key
     * @param array $config
     * @return $this
     */
    public function removeCookie($key, array $config = []) {
        return $this->setCookie($key, null, ['expires' => time()] + $config);
    }

    /**
     * Set the Retry-After header.
     *
     * @uses Titon\Utility\Format
     *
     * @param string|int $length
     * @return $this
     */
    public function retryAfter($length) {
        if (is_string($length)) {
            $length = Format::http($length);
        }

        return $this->setHeader('Retry-After', $length);
    }

    /**
     * {@inheritdoc}
     */
    public function send() {
        $body = $this->getBody();

        // Create an MD5 digest?
        if ($this->getConfig('md5')) {
            $this->setHeader('Content-MD5', base64_encode(pack('H*', md5($body))));
        }

        // Return while in debug
        if ($this->getConfig('debug')) {
            return $body;
        }

        $this->sendHeaders();
        $this->sendBody();

        // @codeCoverageIgnoreStart
        if (function_exists('fastcgi_finish_request')) {
            fastcgi_finish_request();
        }
        // @codeCoverageIgnoreEnd

        return $body;
    }

    /**
     * Output the body by echoing its contents.
     * If a buffer is set, chunk the body into parts.
     *
     * @return $this
     */
    public function sendBody() {
        if ($this->_body && ($buffer = $this->getConfig('buffer'))) {
            $body = str_split($this->_body, $buffer);

            foreach ($body as $chunk) {
                echo $chunk;
            }
        } else {
            echo $this->_body;
        }

        return $this;
    }

    /**
     * Output headers and cookies defined for the current response.
     *
     * @return $this
     * @codeCoverageIgnore
     */
    public function sendHeaders() {
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
    public function setBody($body = null) {
        $this->_body = (string) $body;

        return $this;
    }

    /**
     * Set a cookie with the Set-Cookie header.
     *
     * @param string $key,
     * @param mixed $value
     * @param array $config
     * @return $this
     */
    public function setCookie($key, $value, array $config = []) {
        return $this->addHeader('Set-Cookie', $this->cookies->prepare($key, $value, $config));
    }

    /**
     * {@inheritdoc}
     */
    public function setStatusCode($code) {
        $this->_status = $code;

        return $this->setHeader('Status-Code', $code . ' ' . $this->getReasonPhrase());
    }

    /**
     * Alias for setStatusCode().
     *
     * @param int $code
     * @return $this
     */
    public function statusCode($code) {
        return $this->setStatusCode($code);
    }

    /**
     * Set the Vary header.
     *
     * @param string|array $variances
     * @return $this
     */
    public function vary($variances) {
        return $this->setHeader('Vary', $variances);
    }

    /**
     * Set the WWW-Authenticate header.
     *
     * @param string $scheme
     * @return $this
     */
    public function wwwAuthenticate($scheme) {
        return $this->setHeader('WWW-Authenticate', $scheme);
    }

    /**
     * Convert a resource to XML by instantiating a XmlResponse.
     *
     * @param mixed $data
     * @param string $root
     * @return \Titon\Http\Server\XmlResponse
     */
    public static function xml($data, $root = 'root') {
        return new XmlResponse($data, Http::OK, $root);
    }

}
