<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Route;

use Titon\Route\Exception\MissingPatternException;
use Titon\Utility\Traverse;

/**
 * Represents the skeleton for an individual route. A route matches an internal URL that gets analyzed into multiple parts:
 * module, controller, action, extension, arguments and query parameters. A route can be used to mask a certain URL to
 * another internal destination.
 *
 * @package Titon\Route
 */
class Route {

    /**
     * Pre-defined regex patterns.
     */
    const string ALPHA = '([a-z\_\-\+]+)';
    const string ALNUM = '([a-z0-9\_\-\+]+)';
    const string NUMERIC = '([0-9\.]+)';
    const string WILDCARD = '(.*)';
    const string LOCALE = '([a-z]{2}(?:-[a-z]{2})?)';

    /**
     * The compiled regex pattern.
     *
     * @type string
     */
    protected string $_compiled = '';

    /**
     * Filters to trigger once the route has been matched.
     *
     * @type Vector<string>
     */
    protected Vector<string> $_filters = Vector {};

    /**
     * Patterns matched during the isMatch() process.
     *
     * @type array
     */
    protected array $_matches = [];

    /**
     * The types of acceptable HTTP methods (defaults to all).
     *
     * @type Vector<string>
     */
    protected Vector<string> $_methods = Vector {};

    /**
     * The path to match.
     *
     * @type string
     */
    protected string $_path = '';

    /**
     * Name of tokens to pass as action arguments.
     *
     * @type Vector<string>
     */
    protected Vector<string> $_pass = Vector {};

    /**
     * Custom defined regex patterns.
     *
     * @type Map<string, string>
     */
    protected Map<string, string> $_patterns = Map {};

    /**
     * Collection of route parameters.
     *
     * @type Map<string, mixed>
     */
    protected Map<string, mixed> $_route = Map {};

    /**
     * When true, will only match if under HTTPS.
     *
     * @type bool
     */
    protected bool $_secure = false;

    /**
     * A static route that contains no patterns.
     *
     * @type bool
     */
    protected bool $_static = false;

    /**
     * Custom defined tokens.
     *
     * @type Vector<Map<string, mixed>>
     */
    protected Vector<Map<string, mixed>> $_tokens = Vector {};

    /**
     * The corresponding URL when a match is found.
     *
     * @type string
     */
    protected string $_url;

    /**
     * Store the routing configuration.
     *
     * @uses Titon\Route\Router
     *
     * @param string $path
     * @param string|array $route
     */
    public function __construct(string $path, mixed $route = []) {
        $this->append($path);
        $this->_route = Router::parse($route);
    }

    /**
     * Add a filter by name.
     *
     * @param string $key
     * @return $this
     */
    public function addFilter(string $key): this {
        $this->_filters[] = $key;

        return $this;
    }

    /**
     * Add an HTTP method to match against.
     *
     * @param string $key
     * @return $this
     */
    public function addMethod(string $key): this {
        $this->_methods[] = $key;

        return $this;
    }

    /**
     * Add a regex pattern by token key.
     *
     * @param string $key
     * @param string $value
     * @return $this
     */
    public function addPattern(string $key, string $value): this {
        $this->_patterns[$key] = $value;

        return $this;
    }

    /**
     * Append onto the path. This must be done before compilation.
     *
     * @param string $path
     * @return $this
     */
    public function append(string $path): this {
        $this->_path .= '/' . trim($path, '/');

        return $this;
    }

    /**
     * Compile the given path into a detectable regex pattern.
     *
     * @return string
     * @throws \Titon\Route\Exception\MissingPatternException
     */
    public function compile(): string {
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
                        $this->addPattern($token, $pattern);
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

                    $this->_tokens[] = Map {'token' => $token, 'optional' => $optional};
                }
            } else {
                $this->setStatic(true);
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
     * @return Vector<string>
     */
    public function getFilters(): Vector<string> {
        return $this->_filters;
    }

    /**
     * Return the HTTP method.
     *
     * @return Vector<string>
     */
    public function getMethods(): Vector<string> {
        return $this->_methods;
    }

    /**
     * Return the custom path.
     *
     * @return string
     */
    public function getPath(): string {
        return $this->_path;
    }

    /**
     * Return a param from the route.
     *
     * @uses Titon\Utility\Traverse
     *
     * @param string $key
     * @return mixed
     */
    public function getParam(string $key): mixed {
        return Traverse::get($this->_route, $key);
    }

    /**
     * Return all params.
     *
     * @return Map<string, mixed>
     */
    public function getParams(): Map<string, mixed> {
        return $this->_route;
    }

    /**
     * Return all tokens to be passed as arguments.
     *
     * @return Vector<string>
     */
    public function getPassed(): Vector<string> {
        return $this->_pass;
    }

    /**
     * Return all the patterns used for compiling.
     *
     * @return Map<string, string>
     */
    public function getPatterns(): Map<string, string> {
        return $this->_patterns;
    }

    /**
     * Return the secure configuration.
     *
     * @return bool
     */
    public function getSecure(): bool {
        return $this->_secure;
    }

    /**
     * Return the static configuration.
     *
     * @return bool
     */
    public function getStatic(): bool {
        return $this->_static;
    }

    /**
     * Return the compiled tokens.
     *
     * @return Vector<Map<string, mixed>>
     */
    public function getTokens(): Vector<Map<string, mixed>> {
        return $this->_tokens;
    }

    /**
     * Has the regex pattern been compiled?
     *
     * @return bool
     */
    public function isCompiled(): bool {
        return ($this->_compiled !== '');
    }

    /**
     * Does the URL match the current route?
     *
     * @param string $url
     * @return bool
     */
    public function isMatch(string $url): bool {
        if (!$this->isMethod()) {
            return false;

        } else if (!$this->isSecure()) {
            return false;

        } else if ($this->getPath() === $url) {
            return true;

        } else if (preg_match('/^' . $this->compile() . '$/i', $url, $matches)) {
            $this->match(new Vector($matches));
            return true;
        }

        return false;
    }

    /**
     * Validates the route matches the correct HTTP method.
     *
     * @return bool
     */
    public function isMethod(): bool {
        $method = array_map('strtolower', $this->getMethods());

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
    public function isSecure(): bool {
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
    public function isStatic(): bool {
        return (bool) $this->getStatic();
    }

    /**
     * Receive an array of matched values and apply it to the current route.
     * These matches will equate to tokens, arguments, and other required values.
     *
     * @param Vector<string> $matches
     * @return $this
     */
    public function match(Vector<string> $matches): this {
        $this->_matches = $matches;
        $this->_url = array_shift($matches);

        if ($matches && $this->_tokens) {
            $pass = $this->getPassed();

            foreach ($this->_tokens as $token) {
                $arg = array_shift($matches);

                $this->_route[$token['token']] = $arg;

                if (!isset($this->_route['args'])) {
                    $this->_route['args'] = Vector {};
                }

                if (in_array($token['token'], $pass)) {
                    $this->_route['args'][]  = $arg;
                }
            }
        }

        return $this;
    }

    /**
     * Define multiple tokens to pass as arguments to a dispatched action.
     *
     * @param Vector<string> $tokens
     * @return $this
     */
    public function pass(Vector<string> $tokens): this {
        $this->_pass = $tokens;

        return $this;
    }

    /**
     * Prepend onto the path. This must be done before compilation.
     *
     * @param string $path
     * @return $this
     */
    public function prepend(string $path): this {
        $this->_path = '/' . trim($path, '/') . rtrim($this->_path, '/');

        return $this;
    }

    /**
     * Set the list of filters to process.
     *
     * @param Vector<string> $filters
     * @return $this
     */
    public function setFilters(Vector<string> $filters): this {
        $this->_filters = $filters;

        return $this;
    }

    /**
     * Set the list of HTTP methods to match against.
     *
     * @param Vector<string> $methods
     * @return $this
     */
    public function setMethods(Vector<string> $methods): this {
        $this->_methods = $methods;

        return $this;
    }

    /**
     * Set a mapping of regex patterns to parse URLs with.
     *
     * @param Map<string, string> $patterns
     * @return $this
     */
    public function setPatterns(Map<string, string> $patterns): this {
        $this->_patterns = $patterns;

        return $this;
    }

    /**
     * Set the secure flag.
     *
     * @param bool $secure
     * @return $this
     */
    public function setSecure(bool $secure): this {
        $this->_secure = $secure;

        return $this;
    }

    /**
     * Set the static flag.
     *
     * @param bool $static
     * @return $this
     */
    public function setStatic(bool $static): this {
        $this->_static = $static;

        return $this;
    }

    /**
     * Return the currently matched full URL.
     *
     * @return string
     */
    public function url(): string {
        return $this->_url;
    }

}
