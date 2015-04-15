<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Route;

use Titon\Route\Exception\MissingSegmentException;
use Titon\Route\Exception\MissingTokenException;
use Titon\Utility\Config;
use Titon\Utility\Inflector;
use Titon\Utility\State\Get;
use Titon\Utility\State\Server;

/**
 * The UrlBuilder helps ease the process of building dynamic URLs based on the routes mapped in the Router.
 * Requires a Router instance to be passed through the constructor.
 *
 * @package Titon\Route
 */
class UrlBuilder {

    /**
     * Base folder structure if the application was placed within a directory.
     *
     * @var string
     */
    protected string $base = '/';

    /**
     * Router instance.
     *
     * @var \Titon\Route\Router
     */
    protected Router $router;

    /**
     * The current URL broken up into multiple segments: protocol, host, route, query, base, etc.
     *
     * @var \Titon\Route\SegmentMap
     */
    protected SegmentMap $segments = Map {};

    /**
     * Store the Router instance.
     *
     * @param \Titon\Route\Router $router
     */
    public function __construct(Router $router) {
        $this->router = $router;

        // Determine if app is within a base folder
        $base = dirname(str_replace(Server::get('DOCUMENT_ROOT'), '', Server::get('SCRIPT_FILENAME')));

        if ($base && $base !== '.') {
            $this->base = rtrim(str_replace('\\', '/', $base), '/') ?: '/';
        }

        // Store the current URL and query as router segments
        $this->segments = (new Map(parse_url(Server::get('REQUEST_URI'))))->setAll(Map {
            'scheme' => (Server::get('HTTPS') === 'on') ? 'https' : 'http',
            'query' => Get::all(),
            'host' => Server::get('HTTP_HOST'),
            'port' => Server::get('SERVER_PORT')
        });
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
    <<__Memoize>>
    public function build(string $key, ParamMap $params = Map {}, QueryMap $query = Map {}): string {
        $base = $this->getBase();
        $route = $this->getRouter()->getRoute($key);
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
     * Return the base URL if the app was not placed in the root directory.
     *
     * @return string
     */
    public function getBase(): string {
        return $this->base;
    }

    /**
     * Return the Router instance.
     *
     * @return \Titon\Route\Router
     */
    public function getRouter(): Router {
        return $this->router;
    }

    /**
     * Return a segment by key.
     *
     * @param string $key
     * @return mixed
     * @throws \Titon\Route\Exception\MissingSegmentException
     */
    public function getSegment(string $key): mixed {
        if ($this->segments->contains($key)) {
            return $this->segments[$key];
        }

        throw new MissingSegmentException(sprintf('Routing segment %s does not exist', $key));
    }

    /**
     * Return all segments.
     *
     * @return \Titon\Route\SegmentMap
     */
    public function getSegments(): SegmentMap {
        return $this->segments;
    }

    /**
     * Return the current URL.
     *
     * @return string
     */
    public function url(): string {
        $base = $this->getBase();
        $segments = $this->getSegments();
        $url = (string) $segments['scheme'] . '://' . (string) $segments['host'];

        if ($base !== '/') {
            $url .= $base;
        }

        $url .= (string) $segments['path'];

        if ($segments['query']) {
            $url .= '?' . http_build_query($segments['query']);
        }

        return $url;
    }

}
