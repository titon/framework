<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http\Server;

use Psr\Http\Message\StreamInterface;
use Titon\Http\HeaderMap;
use Titon\Http\StatusCode;
use Titon\Http\Stream\MemoryStream;
use Titon\Http\Stream\ResourceStream;
use InvalidArgumentException;

/**
 * The `ResponseFactory` can be used in the creation of specialized server responses.
 *
 * @package Titon\Http\Server
 */
class ResponseFactory {

    /**
     * Instantiate a new response and package the body stream accordingly.
     *
     * @param mixed $body
     * @param int $status
     * @param \Titon\Http\HeaderMap $headers
     * @return \Titon\Http\Server\Response
     */
    public static function create(mixed $body, int $status = StatusCode::OK, HeaderMap $headers = Map {}): Response {
        if (!$body instanceof StreamInterface) {
            if (is_string($body)) {
                $body = new MemoryStream($body);

            } else if (is_resource($body)) {
                $body = new ResourceStream($body);

            } else {
                throw new InvalidArgumentException('Invalid body type provided');
            }
        }

        return new Response($body, $status, $headers);
    }

    /**
     * Load a file and force a download using HTTP headers.
     *
     * @param string $path
     * @param string $name
     * @param bool $autoEtag
     * @param bool $autoModified
     * @param \Titon\Http\HeaderMap $headers
     * @return \Titon\Http\Server\DownloadResponse
     */
    public static function download(string $path, string $name = '', bool $autoEtag = false, bool $autoModified = true, HeaderMap $headers = Map {}): DownloadResponse {
        $response = new DownloadResponse($path, StatusCode::OK, $headers);

        if ($name) {
            $response = $response->contentDisposition($name);
        }

        if ($autoEtag) {
            $response = $response->etag(sha1_file($path));
        }

        if ($autoModified) {
            $response = $response->lastModified(filemtime($path));
        }

        return $response;
    }

    /**
     * Convert a resource to JSON by instantiating a JsonResponse.
     * Can optionally pass encoding options, and a JSONP callback.
     *
     * @param mixed $data
     * @param int $status
     * @param int $flags
     * @param string $callback
     * @param \Titon\Http\HeaderMap $headers
     * @return \Titon\Http\Server\JsonResponse
     */
    public static function json(mixed $data, int $status = StatusCode::OK, int $flags = -1, string $callback = '', HeaderMap $headers = Map {}): JsonResponse {
        return new JsonResponse($data, $status, $flags, $callback, $headers);
    }

    /**
     * Redirect to another URL by instantiating a new RedirectResponse.
     *
     * @param string $url
     * @param int $status
     * @param \Titon\Http\HeaderMap $headers
     * @return \Titon\Http\Server\RedirectResponse
     */
    public static function redirect(string $url, int $status = StatusCode::FOUND, HeaderMap $headers = Map {}): RedirectResponse {
        return new RedirectResponse($url, $status, $headers);
    }

    /**
     * Redirect to the previous page using the REFERER HTTP header.
     *
     * @param int $status
     * @param \Titon\Http\HeaderMap $headers
     * @return \Titon\Http\Server\RedirectResponse
     */
    public static function redirectBack(int $status = StatusCode::FOUND, HeaderMap $headers = Map {}): RedirectResponse {
        return static::redirect(Request::createFromGlobals()->getReferrer(), $status, $headers);
    }

    /**
     * Redirect to the home page.
     *
     * @param int $status
     * @param \Titon\Http\HeaderMap $headers
     * @return \Titon\Http\Server\RedirectResponse
     */
    public static function redirectHome(int $status = StatusCode::FOUND, HeaderMap $headers = Map {}): RedirectResponse {
        return static::redirect('/', $status, $headers);
    }

    /**
     * Refresh the current page.
     *
     * @param int $status
     * @param \Titon\Http\HeaderMap $headers
     * @return \Titon\Http\Server\RedirectResponse
     */
    public static function refresh(int $status = StatusCode::FOUND, HeaderMap $headers = Map {}): RedirectResponse {
        return static::redirect(Request::createFromGlobals()->getUrl(), $status, $headers);
    }

    /**
     * Convert a resource to XML by instantiating an XmlResponse.
     *
     * @param mixed $data
     * @param int $status
     * @param string $root
     * @param \Titon\Http\HeaderMap $headers
     * @return \Titon\Http\Server\XmlResponse
     */
    public static function xml(mixed $data, int $status = StatusCode::OK, string $root = 'root', HeaderMap $headers = Map {}): XmlResponse {
        return new XmlResponse($data, $status, $root, $headers);
    }
}
