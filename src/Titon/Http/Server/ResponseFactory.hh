<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http\Server;

use Psr\Http\Message\StreamInterface;
use Titon\Http\StatusCode;
use Titon\Http\Stream\MemoryStream;
use Titon\Http\Stream\ResourceStream;
use InvalidArgumentException;

/**
 * The `ResponseFactory` can be used in the creation of specialized responses.
 *
 * @package Titon\Http\Server
 */
class ResponseFactory {

    /**
     * Instantiate a new response and package the body stream accordingly.
     *
     * @param mixed $body
     * @param int $status
     * @return \Titon\Http\Server\Response
     */
    public static function create(mixed $body, int $status = StatusCode::OK): Response {
        if (!$body instanceof StreamInterface) {
            if (is_string($body)) {
                $body = new MemoryStream($body);

            } else if (is_resource($body)) {
                $body = new ResourceStream($body);

            } else {
                throw new InvalidArgumentException('Invalid body type provided');
            }
        }

        return new Response($body, $status);
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
        $response = new DownloadResponse($path, StatusCode::OK);

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
     * @return \Titon\Http\Server\JsonResponse
     */
    public static function json(mixed $data, int $status = StatusCode::OK, int $flags = 0, string $callback = ''): JsonResponse {
        return new JsonResponse($data, $status, $flags, $callback);
    }

    /**
     * Redirect to another URL by instantiating a new RedirectResponse.
     *
     * @param string $url
     * @param int $status
     * @return \Titon\Http\Server\RedirectResponse
     */
    public static function redirect(string $url, int $status = StatusCode::FOUND): RedirectResponse {
        return new RedirectResponse($url, $status);
    }

    /**
     * Convert a resource to XML by instantiating an XmlResponse.
     *
     * @param mixed $data
     * @param int $status
     * @param string $root
     * @return \Titon\Http\Server\XmlResponse
     */
    public static function xml(mixed $data, int $status = StatusCode::OK, string $root = 'root'): XmlResponse {
        return new XmlResponse($data, $status, $root);
    }
}
