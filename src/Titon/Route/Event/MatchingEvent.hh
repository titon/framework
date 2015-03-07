<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Route\Event;

use Titon\Event\Event;
use Titon\Route\Router;

/**
 * The route event that occurs before the route is matched.
 *
 * @package Titon\Route\Event
 */
class MatchingEvent extends Event {

    /**
     * The router instance.
     *
     * @var \Titon\Route\Router
     */
    protected Router $router;

    /**
     * The URL to match.
     *
     * @var string
     */
    protected string $url;

    /**
     * Store the event settings.
     *
     * @param \Titon\Route\Router $router
     * @param string $url
     */
    public function __construct(Router $router, string $url) {
        $this->router = $router;
        $this->url = $url;

        parent::__construct('route.matching');
    }

    /**
     * Return the router.
     *
     * @return \Titon\Route\Router
     */
    public function getRouter(): Router {
        return $this->router;
    }

    /**
     * Return the URL to match.
     *
     * @return string
     */
    public function getUrl(): string {
        return $this->url;
    }

}
