<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Route;

use Titon\Common\Cacheable;
use Titon\Common\FactoryAware;
use Titon\Route\Exception\MissingTokenException;
use Titon\Utility\Config;
use Titon\Utility\Inflector;

/**
 * The UrlBuilder helps ease the process of building dynamic URLs based on the routes mapped in the Router.
 * Requires a Router instance to be passed through the constructor.
 *
 * @package Titon\Route
 */
class UrlBuilder {
    use Cacheable, FactoryAware;

    /**
     * Router instance.
     *
     * @var \Titon\Route\Router
     */
    protected Router $_router;

    /**
     * Store the Router instance.
     *
     * @param \Titon\Route\Router $router
     */
    public function __construct(Router $router) {
        $this->_router = $router;
    }

    /**
     * Builds a URL for a route by replacing all tokens with custom defined parameters.
     * An optional query string and fragment can be also be defined.
     *
     * @param string $key
     * @param \Titon\Route\ParamMap $params
     * @param \Titon\Route\QueryMap $query
     * @return string
     * @throws \Titon\Route\Exception\MissingTokenException
     */
    public function build(string $key, ParamMap $params = Map {}, QueryMap $query = Map {}): string {
        $url = (string) $this->cache([__METHOD__, $key, $params], function(UrlBuilder $builder) use ($key, $params) {
            $router = $builder->getRouter();
            $route = $router->getRoute($key);
            $base = $router->base();
            $url = str_replace([']', ')', '>'], '}', str_replace(['[', '(', '<'], '{', $route->getPath()));

            // Set the locale if it is missing
            if (!$params->contains('locale')) {
                $params['locale'] = Config::get('titon.locale.current');
            }

            // Replace tokens in the path with values from the parameters
            foreach ($route->getTokens() as $token) {
                $tokenKey = $token['token'];

                if ($params->contains($tokenKey) || $token['optional']) {
                    $url = str_replace(sprintf('{%s}', $tokenKey . ($token['optional'] ? '?' : '')), Inflector::route((string) $params->get($tokenKey) ?: ''), $url);

                } else {
                    throw new MissingTokenException(sprintf('Missing %s parameter for the %s route', $tokenKey, $key));
                }
            }

            // Prepend base folder
            if ($base !== '/') {
                $url = $base . $url;
            }

            // Trim trailing slash
            if ($url !== '/') {
                $url = rtrim($url, '/');
            }

            return $url;
        });

        // Append query string and fragment
        $fragment = $query->get('#');

        $query->remove('#');

        if ($query) {
            $url .= '?' . http_build_query($query);
        }

        if ($fragment) {
            $url .= '#' . (($fragment instanceof Traversable) ? http_build_query($fragment) : urlencode($fragment));
        }

        return $url;
    }

    /**
     * Return the Router instance.
     *
     * @return \Titon\Route\Router
     */
    public function getRouter(): Router {
        return $this->_router;
    }

    /**
     * Return the current URL.
     *
     * @return string
     */
    public function url(): string {
        return $this->cache(__METHOD__, function(UrlBuilder $builder) {
            $router = $builder->getRouter();
            $segments = $router->getSegments();
            $base = $router->base();
            $url = (string) $segments['scheme'] . '://' . (string) $segments['host'];

            if ($base !== '/') {
                $url .= $base;
            }

            $url .= (string) $segments['path'];

            if ($segments['query']) {
                $url .= '?' . http_build_query($segments['query']);
            }

            return $url;
        });
    }

}