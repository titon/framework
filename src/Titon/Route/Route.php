<?hh //strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Route;

use Titon\Common\Base;
use Titon\Route\Exception\MissingPatternException;
use Titon\Utility\Hash;

/**
 * Represents the skeleton for an individual route. A route matches an internal URL that gets analyzed into multiple parts:
 * module, controller, action, extension, arguments and query parameters. A route can be used to mask a certain URL to
 * another internal destination.
 *
 * @package Titon\Route
 */
class Route extends Base {

    /**
     * Pre-defined regex patterns.
     */
    const ALPHA = '([a-z\_\-\+]+)';
    const ALNUM = '([a-z0-9\_\-\+]+)';
    const NUMERIC = '([0-9\.]+)';
    const WILDCARD = '(.*)';
    const LOCALE = '([a-z]{2}(?:-[a-z]{2})?)';

    /**
     * Configuration.
     *
     * @type array {
     *      @type bool $secure      When true, will only match if under HTTPS
     *      @type bool $static      A static route that contains no patterns
     *      @type array $method     The types of acceptable HTTP methods (defaults to all)
     *      @type array $patterns   Custom defined regex patterns
     *      @type array $pass       Name of tokens to pass as action arguments
     *      @type array $filters    Filters to trigger once the route has been matched
     *      @type string $locale    Fallback locale to redirect to
     * }
     */
    protected $_config = [
        'secure' => false,
        'static' => false,
        'method' => [],
        'patterns' => [],
        'pass' => [],
        'filters' => [],
        'locale' => ''
    ];

    /**
     * The compiled regex pattern.
     *
     * @type string
     */
    protected $_compiled;

    /**
     * Unique name of this route.
     *
     * @type string
     */
    protected $_key;

    /**
     * Patterns matched during the isMatch() process.
     *
     * @type array
     */
    protected $_matches = [];

    /**
     * The path to match.
     *
     * @type string
     */
    protected $_path = '';

    /**
     * Collection of route parameters.
     *
     * @type array
     */
    protected $_route = [];

    /**
     * Custom defined tokens.
     *
     * @type array
     */
    protected $_tokens = [];

    /**
     * The corresponding URL when a match is found.
     *
     * @type string
     */
    protected $_url;

    /**
     * Store the routing configuration.
     *
     * @uses Titon\Route\Router
     *
     * @param string $path
     * @param string|array $route
     * @param array $config
     */
    public function __construct($path, $route = [], array $config = []) {
        $this->append($path);
        $this->_route = Router::parse($route);

        parent::__construct($config);
    }

    /**
     * Append onto the path. This must be done before compilation.
     *
     * @param string $path
     * @return $this
     */
    public function append($path) {
        $this->_path .= '/' . trim($path, '/');

        return $this;
    }

    /**
     * Compile the given path into a detectable regex pattern.
     *
     * @return string
     * @throws \Titon\Route\Exception\MissingPatternException
     */
    public function compile() {
        if ($this->isCompiled()) {
            return $this->_compiled;
        }

        $path = $this->getPath();
        $compiled = str_replace(['/', '.'], ['\/', '\.'], $path);
        $patterns = $this->getPatterns();

        if (!$this->isStatic()) {
            $tokens = [];

            // Match regex pattern tokens first
            preg_match_all('/(\<)([^\<\>]+)(\>)/i', $path, $matches, PREG_SET_ORDER);
            $tokens = array_merge($tokens, $matches);

            // Then match regular tokens
            preg_match_all('/(\{|\(|\[)([a-z0-9\?]+)(\}|\)|\])/i', $path, $matches, PREG_SET_ORDER);
            $tokens = array_merge($tokens, $matches);

            if ($tokens) {
                foreach ($tokens as $match) {
                    $chunk = $match[0];
                    $open = isset($match[1]) ? $match[1] : ''; // opening brace
                    $token = isset($match[2]) ? $match[2] : ''; // token
                    $close = isset($match[3]) ? $match[3] : ''; // closing brace
                    $optional = false;

                    // Is the token optional
                    if (substr($token, -1) === '?') {
                        $optional = true;
                        $token = substr($token, 0, strlen($token) - 1);
                    }

                    // Pattern exists
                    if (strpos($token, ':') !== false) {
                        list($token, $pattern) = explode(':', $token, 2);

                        $patterns[$token] = $pattern;
                        $this->setConfig('patterns.' . $token, $pattern);
                    }

                    if ($open === '{' && $close === '}') {
                        $pattern = self::ALPHA;

                    } else if ($open === '[' && $close === ']') {
                        $pattern = self::NUMERIC;

                    } else if ($open === '(' && $close === ')') {
                        $pattern = self::WILDCARD;

                    } else if ($open === '<' && $close === '>' && isset($patterns[$token])) {
                        $pattern = '(' . trim($patterns[$token], '()') . ')';

                    } else {
                        throw new MissingPatternException(sprintf('Unknown pattern for %s token', $token));
                    }

                    // Apply optional flag by altering chunk and pattern
                    if ($optional) {
                        $chunk = '\/' . $chunk;
                        $pattern = '(?:\/' . $pattern . ')?';
                    }

                    $compiled = str_replace($chunk, $pattern, $compiled);

                    $this->_tokens[] = ['token' => $token, 'optional' => $optional];
                }
            } else {
                $this->setConfig('static', true);
            }
        }

        // Append a check for a trailing slash
        if ($path !== '/') {
            $compiled .= '\/?';
        }

        // Save the compiled regex
        $this->_compiled = $compiled;

        return $this->_compiled;
    }

    /**
     * Return all filters.
     *
     * @return array
     */
    public function getFilters() {
        return array_unique((array) $this->getConfig('filters'));
    }

    /**
     * Return the HTTP method.
     *
     * @return array|string
     */
    public function getMethod() {
        return (array) $this->getConfig('method');
    }

    /**
     * Return the custom path.
     *
     * @return string
     */
    public function getPath() {
        return $this->_path;
    }

    /**
     * Return a param from the route.
     *
     * @uses Titon\Utility\Hash
     *
     * @param string $key
     * @return string
     */
    public function getParam($key) {
        return Hash::get($this->_route, $key);
    }

    /**
     * Return all params.
     *
     * @return array
     */
    public function getParams() {
        return $this->_route;
    }

    /**
     * Return all tokens to be passed as arguments.
     *
     * @return array
     */
    public function getPassed() {
        return array_unique((array) $this->getConfig('pass'));
    }

    /**
     * Return all the patterns used for compiling.
     *
     * @return array
     */
    public function getPatterns() {
        return $this->getConfig('patterns');
    }

    /**
     * Return the secure configuration.
     *
     * @return bool
     */
    public function getSecure() {
        return $this->getConfig('secure');
    }

    /**
     * Return the static configuration.
     *
     * @return bool
     */
    public function getStatic() {
        return $this->getConfig('static');
    }

    /**
     * Return the compiled tokens.
     *
     * @return array
     */
    public function getTokens() {
        return $this->_tokens;
    }

    /**
     * Has the regex pattern been compiled?
     *
     * @return bool
     */
    public function isCompiled() {
        return ($this->_compiled !== null);
    }

    /**
     * Does the URL match the current route?
     *
     * @param string $url
     * @return bool
     */
    public function isMatch($url) {
        if (!$this->isMethod()) {
            return false;

        } else if (!$this->isSecure()) {
            return false;

        } else if ($this->getPath() === $url) {
            return true;

        } else if (preg_match('/^' . $this->compile() . '$/i', $url, $matches)) {
            $this->match($matches);
            return true;
        }

        return false;
    }

    /**
     * Validates the route matches the correct HTTP method.
     *
     * @return bool
     */
    public function isMethod() {
        $method = array_map('strtolower', $this->getMethod());

        if ($method && !in_array(strtolower($_SERVER['REQUEST_METHOD']), $method, true)) {
            return false;
        }

        return true;
    }

    /**
     * Validates the route matches a secure connection.
     *
     * @return bool
     */
    public function isSecure() {
        $isSecure = (
            isset($_SERVER['HTTPS']) && $_SERVER['HTTPS'] === 'on' ||
            isset($_SERVER['SERVER_PORT']) && $_SERVER['SERVER_PORT'] == 443
        );

        // Only validate if the secure flag is true
        if ($this->getSecure() && !$isSecure) {
            return false;
        }

        return true;
    }

    /**
     * Is the route static (no regex patterns)?
     *
     * @return bool
     */
    public function isStatic() {
        return (bool) $this->getStatic();
    }

    /**
     * Receive an array of matched values and apply it to the current route.
     * These matches will equate to tokens, arguments, and other required values.
     *
     * @param array $matches
     * @return $this
     */
    public function match(array $matches) {
        $this->_matches = $matches;
        $this->_url = array_shift($matches);

        if ($matches && $this->_tokens) {
            $pass = (array) $this->getPassed();

            foreach ($this->_tokens as $token) {
                $arg = array_shift($matches);

                $this->_route[$token['token']] = $arg;

                if (in_array($token['token'], $pass)) {
                    $this->_route['args'][]  = $arg;
                }
            }
        }

        return $this;
    }

    /**
     * Prepend onto the path. This must be done before compilation.
     *
     * @param string $path
     * @return $this
     */
    public function prepend($path) {
        $this->_path = '/' . trim($path, '/') . rtrim($this->_path, '/');

        return $this;
    }

    /**
     * Return the currently matched full URL.
     *
     * @return string
     */
    public function url() {
        return $this->_url;
    }

}
