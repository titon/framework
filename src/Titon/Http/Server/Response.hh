<?hh // partial
// Because of PSR HTTP Message
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http\Server;

use Psr\Http\Message\StreamInterface;
use Titon\Http\AbstractResponse;
use Titon\Http\Cookie;
use Titon\Http\HeaderMap;
use Titon\Http\Http;
use Titon\Http\StatusCode;
use Titon\Utility\Config;
use Titon\Utility\Format;
use Titon\Utility\MimeType;
use Titon\Utility\Number;
use Titon\Utility\Str;
use Titon\Utility\Time;
use InvalidArgumentException;

/**
 * The Response object handles the collection and output of data to the browser.
 * It stores a list of HTTP headers, the content body, the content type, and an associated status code.
 *
 * @package Titon\Http\Server
 */
class Response extends AbstractResponse {

    /**
     * Set body and status during initialization.
     *
     * @param \Psr\Http\Message\StreamInterface $body
     * @param int $status
     * @param \Titon\Http\HeaderMap $headers
     */
    public function __construct(?StreamInterface $body = null, int $status = StatusCode::OK, HeaderMap $headers = Map {}) {
        parent::__construct($body, $headers);

        $this->status = $status;
    }

    /**
     * Set the Accept-Ranges header.
     *
     * @param string $range
     * @return $this
     */
    public function acceptRanges(string $range = 'bytes'): this {
        return $this->withHeader('Accept-Ranges', $range);
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

        return $this->withHeader('Age', $time);
    }

    /**
     * Set the Allow header.
     *
     * @param Set<string> $methods
     * @return $this
     */
    public function allow(Set<string> $methods): this {
        $methods = array_intersect(array_map(fun('strtoupper'), $methods), Http::getMethodTypes());

        return $this->withHeader('Allow', $methods);
    }

    /**
     * Alias for `withBody()`.
     *
     * @param \Psr\Http\Message\StreamInterface $body
     * @return $this
     */
    public function body(StreamInterface $body): this {
        return $this->withBody($body);
    }

    /**
     * Force the clients browser to cache the current request.
     *
     * @param string $directive
     * @param int|string $expires
     * @param bool $proxy
     * @param Map<string, mixed> $options
     * @return $this
     * @throws \Titon\Common\Exception\InvalidArgumentException
     */
    public function cache(string $directive, mixed $expires = '+24 hours', bool $proxy = true, Map<string, mixed> $options = Map {}): this {
        $expires = Time::toUnix($expires);
        $self = $this;

        if ($directive === 'none') {
            $control = Map {
                'no-cache' => true,
                'no-store' => true,
                'must-revalidate' => true
            };

            if ($proxy) {
                $control['proxy-revalidate'] = true;
            }

            $self = $self->withHeader('Pragma', 'no-cache');

        } else if ($directive === 'public' || $directive === 'private') {
            $control = Map {};
            $control[$directive] = true;
            $ttl = $expires - time();

            if ($ttl > 0) {
                $control['max-age'] = $ttl;

                if ($proxy) {
                    $control['s-maxage'] = $ttl;
                }
            }
        } else {
            throw new InvalidArgumentException(sprintf('Invalid cache directive %s', $directive));
        }

        return $self
            ->expires($expires)
            ->cacheControl($control->setAll($options));
    }

    /**
     * Set the Cache-Control header.
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

        return $this->withHeader('Cache-Control', $header);
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

        return $this->withHeader('Connection', $status);
    }

    /**
     * Set the Content-Disposition header.
     *
     * @param string $file
     * @param string $type
     * @return $this
     * @throws \Titon\Common\Exception\InvalidArgumentException
     */
    public function contentDisposition(string $file, string $type = 'attachment'): this {
        if ($type !== 'attachment' && $type !== 'inline') {
            throw new InvalidArgumentException('Disposition type must be either "attachment" or "inline"');
        }

        return $this->withHeader('Content-Disposition', sprintf('%s; filename="%s"', $type, $file));
    }

    /**
     * Set the Content-Encoding header.
     *
     * @param string $encoding
     * @return $this
     */
    public function contentEncoding(string $encoding): this {
        return $this->withHeader('Content-Encoding', $encoding);
    }

    /**
     * Set the Content-Language header.
     *
     * @param string $locales
     * @return $this
     */
    public function contentLanguage(string $locales): this {
        return $this->withHeader('Content-Language', $locales);
    }

    /**
     * Set the Content-Location header.
     *
     * @param string $location
     * @return $this
     */
    public function contentLocation(string $location): this {
        return $this->withHeader('Content-Location', $location);
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
        if ($this->status >= 300 && $this->status <= 399) {
            return $this;
        }

        if (!is_numeric($length)) {
            $length = Number::bytesFrom((string) $length);
        }

        return $this->withHeader('Content-Length', $length);
    }

    /**
     * Set the Content-MD5 header.
     *
     * @param string $content
     * @return $this
     */
    public function contentMD5(string $content): this {
        return $this->withHeader('Content-MD5', $content);
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
        return $this->withHeader('Content-Range', sprintf('bytes %s-%s/%s', $start, $end, $size));
    }

    /**
     * Set the Content-Type header.
     *
     * @param string $type
     * @return $this
     */
    public function contentType(string $type): this {
        if (in_array($this->status, [StatusCode::NOT_MODIFIED, StatusCode::NO_CONTENT])) {
            return $this;
        }

        return $this->withHeader('Content-Type', $this->validateContentType($type));
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
        return $this->withHeader('Date', Format::http($time));
    }

    /**
     * Set the ETag header. If the tag is null, generate a hash based off the current URL.
     *
     * @param string $tag
     * @param bool $weak
     * @return $this
     */
    public function etag(string $tag, bool $weak = false): this {
        return $this->withHeader('ETag', sprintf('%s"%s"', ($weak ? 'W/' : ''), $tag));
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
        return $this->withHeader('Expires', Format::http($expires));
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
        return $this->withHeader('Last-Modified', Format::http($time));
    }

    /**
     * Set the Location header.
     *
     * @param string $url
     * @return $this
     */
    public function location(string $url): this {
        return $this->withHeader('Location', $url);
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
        return $this->statusCode(StatusCode::NOT_MODIFIED)
            ->withoutHeader('Allow')
            ->withoutHeader('Content-Disposition')
            ->withoutHeader('Content-Encoding')
            ->withoutHeader('Content-Language')
            ->withoutHeader('Content-Length')
            ->withoutHeader('Content-MD5')
            ->withoutHeader('Content-Type')
            ->withoutHeader('Last-Modified');
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
        return $this->setCookie($name, '', null, time(), $path, $domain, $httpOnly, $secure);
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

        return $this->withHeader('Retry-After', $length);
    }

    /**
     * Set a cookie with the Set-Cookie header.
     *
     * @param string $name
     * @param string $value
     * @param \Titon\Crypto\Cipher $cipher
     * @param mixed $expires
     * @param string $path
     * @param string $domain
     * @param bool $httpOnly
     * @param bool $secure
     * @return $this
     */
    public function setCookie(string $name, string $value, ?Cipher $cipher = null, mixed $expires = 0, string $path = '/', string $domain = '', bool $httpOnly = true, bool $secure = false): this {
        return $this->withAddedHeader('Set-Cookie', (string) (new Cookie($name, '', $expires, $path, $domain, $httpOnly, $secure))->setValue($value, $cipher));
    }

    /**
     * Alias for `withStatus()`.
     *
     * @param int $code
     * @return $this
     */
    public function statusCode(int $code): this {
        return $this->withStatus($code);
    }

    /**
     * Set the Vary header.
     *
     * @param string $variances
     * @return $this
     */
    public function vary(string $variances): this {
        return $this->withHeader('Vary', $variances);
    }

    /**
     * Set the WWW-Authenticate header.
     *
     * @param string $scheme
     * @return $this
     */
    public function wwwAuthenticate(string $scheme): this {
        return $this->withHeader('WWW-Authenticate', $scheme);
    }

    /**
     * Validate a content type and return the entire mime type string.
     * If an extension is passed, attempt to determine the mime type.
     * If the mime type supports a charset, include it.
     *
     * @uses Titon\Utility\Config
     * @uses Titon\Utility\MimeType
     * @uses Titon\Utility\Str
     *
     * @param string $type
     * @return $this
     */
    protected function validateContentType(string $type): string {
        if (strpos($type, '/') === false) {
            $type = MimeType::getTypeByExt($type);
        }

        $charset = Config::encoding();

        if ($charset && (Str::startsWith($type, MimeType::TEXT) ||
                in_array($type, ['application/javascript', 'application/json', 'application/xml', 'application/rss+xml']))) {
            $type .= '; charset=' . $charset;
        }

        return $type;
    }
}
