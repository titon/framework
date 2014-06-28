<?hh
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http\Server;

use Titon\Http\Http;
use \Closure;

/**
 * The Redirector bundles convenience methods for redirecting to new URLs using HTTP headers.
 * This class should only be used in instances where the Response object cannot be modified directly.
 * In all other cases, Response::redirect() should be used.
 *
 * @package Titon\Http\Server
 */
class Redirector {

    /**
     * Whether to exit() or return when redirecting.
     *
     * @type bool
     */
    public static $exit = true;

    /**
     * Redirect to the previous page using the REFERER HTTP header.
     *
     * @param int $status
     * @param \Closure $callback
     * @return \Titon\Http\Server\Response
     */
    public static function back($status = Http::FOUND, Closure $callback = null) {
        return static::to(Request::createFromGlobals()->getReferrer(), $status, $callback);
    }

    /**
     * Redirect to the home page.
     *
     * @param int $status
     * @param \Closure $callback
     * @return \Titon\Http\Server\Response
     */
    public static function home($status = Http::FOUND, Closure $callback = null) {
        return static::to('/', $status, $callback);
    }

    /**
     * Refresh the current page.
     *
     * @param int $status
     * @param \Closure $callback
     * @return \Titon\Http\Server\Response
     */
    public static function refresh($status = Http::FOUND, Closure $callback = null) {
        return static::to(Request::createFromGlobals()->getUrl(), $status, $callback);
    }

    /**
     * Redirect to a URL with optional status code (defaults to Http::FOUND FOUND).
     * If a callback is defined, pass the response object for modification.
     *
     * @param string $url
     * @param int $status
     * @param \Closure $callback
     * @return \Titon\Http\Server\Response
     */
    public static function to($url, $status = Http::FOUND, Closure $callback = null) {
        $response = new RedirectResponse($url, $status);

        if ($callback) {
            call_user_func($callback, $response);
        }

        // We should exit early in case we're in a closure
        // Or hooking into some process within the script
        // @codeCoverageIgnoreStart
        if (static::$exit) {
            $response->send();
            exit();
        }
        // @codeCoverageIgnoreEnd

        return $response;
    }

}