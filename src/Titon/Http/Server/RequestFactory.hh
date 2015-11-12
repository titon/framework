<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http\Server;

use Psr\Http\Message\StreamInterface;
use Psr\Http\Message\UriInterface;
use Titon\Http\Cookie as CookieObject;
use Titon\Http\HeaderMap;
use Titon\Http\Stream\ResourceStream;
use Titon\Http\Uri;
use Titon\Utility\State\Cookie;
use Titon\Utility\State\Files;
use Titon\Utility\State\Get;
use Titon\Utility\State\GlobalMap;
use Titon\Utility\State\Post;
use Titon\Utility\State\Server;

/**
 * The `RequestFactory` can be used in the creation of specialized server requests.
 *
 * @package Titon\Http\Server
 */
class RequestFactory {

    public static function applyGlobals(Request $request): Request {
        if ($cookie = static::getCookieParams()) {
            $request = $request->withCookieParams($cookie);
        }

        if ($query = static::getQueryParams()) {
            $request = $request->withQueryParams($query);
        }

        if ($post = static::getPostParams()) {
            $request = $request->withParsedBody($post);
        }

        if ($files = static::getUploadedFiles()) {
            $request = $request->withUploadedFiles($files);
        }

        return $request;
    }

    /**
     * Instantiate a new request and package the URI and body stream accordingly.
     *
     * @param \Titon\Utility\State\GlobalMap $server
     * @param \Psr\Http\Message\UriInterface|string $uri
     * @param string $method
     * @param \Titon\Http\HeaderMap $headers
     * @return \Titon\Http\Server\Request
     */
    public static function create(GlobalMap $server, mixed $uri, string $method = 'GET', HeaderMap $headers = Map {}): Request {
        if (!$uri instanceof UriInterface) {
            $uri = new Uri((string) $uri);
        }

        $body = new ResourceStream(fopen('php://input', 'r+'));

        $serverHeaders = static::extractHeaders($server);
        $serverHeaders->setAll($headers);

        return new Request($server, $uri, $method, $body, $serverHeaders);
    }

    /**
     * Create a request object based off the current super globals.
     *
     * @return \Titon\Http\Server\Request
     */
    public static function createFromGlobals(): Request {
        return static::applyGlobals(
            static::create(Server::all(), static::getUrl(), static::getMethod())
        );
    }

    /**
     * Extract all HTTP headers from the server super global.
     *
     * @param \Titon\Utility\State\GlobalMap $params
     * @return \Titon\Http\HeaderMap
     */
    public static function extractHeaders(GlobalMap $params): HeaderMap {
        $headers = Map {};

        foreach ($params as $key => $value) {
            if ($key === 'HTTP_COOKIE') {
                continue;

            } else if (substr($key, 0, 5) === 'HTTP_') {
                $key = substr($key, 5);

            } else if (substr($key, 0, 8) !== 'CONTENT_') {
                continue;
            }

            $headers[$key] = explode(',', (string) $value);
        }

        return $headers;
    }

    /**
     * Return all cookies wrapped in a `Cookie` object.
     *
     * @return array<string, mixed>
     */
    public static function getCookieParams(): array<string, mixed> {
        $cookies = [];

        foreach (Cookie::all() as $key => $value) {
            $cookies[$key] = new CookieObject($key, (string) $value);
        }

        return $cookies;
    }

    /**
     * Return all query parameters.
     *
     * @return array<string, mixed>
     */
    public static function getQueryParams(): array<string, mixed> {
        return Get::all()->toArray();
    }

    /**
     * Return all post parameters.
     *
     * @return array<string, mixed>
     */
    public static function getPostParams(): array<string, mixed> {
        return Post::all()->toArray();
    }

    /**
     * Return all files wrapped in a `UploadedFile` object.
     *
     * @return array<string, mixed>
     */
    public static function getUploadedFiles(): array<string, mixed> {
        return Files::all()->toArray();
    }

    /**
     * Determine the current HTTP request method based on the server parameters.
     *
     * @return string
     */
    public static function getMethod(): string {
        $method = Server::get('REQUEST_METHOD', 'GET');

        if ($method === 'POST') {
            if ($override = Server::get('HTTP_X_METHOD_OVERRIDE')) {
                $method = $override;

            } else if ($override = Post::get('_method')) {
                $method = $override;
            }
        }

        return strtoupper($method);
    }

    /**
     * Determine the current URL of the request based on the server parameters.
     *
     * @return string
     */
    public static function getUrl(): string {
        $script = str_replace(Server::get('DOCUMENT_ROOT'), '', Server::get('SCRIPT_FILENAME'));
        $base = dirname($script);
        $url = '/';

        // Proper URL defined by the web server
        if ($path = Server::get('PATH_INFO')) {
            $url = (string) $path;

            // Strip off the query string if it exists
        } else if ($path = Server::get('REQUEST_URI')) {
            $url = explode('?', (string) $path)[0];

            // Remove the base folder and index file
        } else if ($path = Server::get('PHP_SELF')) {
            $url = str_replace($script, '', (string) $path);
        }

        if ($base !== '/' && $base !== '\\') {
            $url = $base . $url;
        }

        return $url;
    }

}
