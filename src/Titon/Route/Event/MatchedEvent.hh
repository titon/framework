<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Route\Event;

use Titon\Event\Event;
use Titon\Route\Route;
use Titon\Route\Router;

/**
 * The route event that occurs after the route is matched.
 * The matched route is passed to the event.
 *
 * @package Titon\Route\Event
 */
class MatchedEvent extends Event {

    /**
     * The router instance.
     *
     * @var \Titon\Route\Router
     */
    protected Router $router;

    /**
     * The matched route.
     *
     * @var \Titon\Route\Route
     */
    protected Route $route;

    /**
     * Store the event settings.
     *
     * @param \Titon\Route\Router $router
     * @param \Titon\Route\Route $route
     */
    public function __construct(Router $router, Route $route) {
        $this->router = $router;
        $this->route = $route;

        parent::__construct('route.matched');
    }

    /**
     * Return the route.
     *
     * @return \Titon\Route\Route
     */
    public function getRoute(): Route {
        return $this->route;
    }

    /**
     * Return the router.
     *
     * @return \Titon\Route\Router
     */
    public function getRouter(): Router {
        return $this->router;
    }

}
