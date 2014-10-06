<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Route;

use Titon\Route\Exception\MissingPatternException;

type FilterList = Vector<string>;
type MethodList = Vector<string>;
type ParamMap = Map<string, mixed>;
type PatternMap = Map<string, string>;
type Token = shape('token' => string, 'optional' => bool);
type TokenList = Vector<Token>;

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
     * The action to execute if this route is matched.
     *
     * @type \Titon\Route\Action
     */
    protected Action $_action;

    /**
     * The compiled regex pattern.
     *
     * @type string
     */
    protected string $_compiled = '';

    /**
     * Filters to trigger once the route has been matched.
     *
     * @type \Titon\Route\FilterList
     */
    protected FilterList $_filters = Vector {};

    /**
     * The types of acceptable HTTP methods (defaults to all).
     *
     * @type \Titon\Route\MethodList
     */
    protected MethodList $_methods = Vector {};

    /**
     * Collection of route parameters.
     *
     * @type \Titon\Route\ParamMap
     */
    protected ParamMap $_params = Map {};

    /**
     * The path to match.
     *
     * @type string
     */
    protected string $_path = '';

    /**
     * Custom defined regex patterns.
     *
     * @type \Titon\Route\PatternMap
     */
    protected PatternMap $_patterns = Map {};

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
     * @type \Titon\Route\TokenList
     */
    protected TokenList $_tokens = Vector {};

    /**
     * The corresponding URL when a match is found.
     *
     * @type string
     */
    protected string $_url = '';

    /**
     * Store the routing configuration.
     *
     * @uses Titon\Route\Router
     *
     * @param string $path
     * @param string $action
     */
    public function __construct(string $path, string $action) {
        $this->_action = Router::parseAction($action);
        $this->append($path);
    }

    /**
     * Add a filter by name.
     *
     * @param string $filter
     * @return $this
     */
    public function addFilter(string $filter): this {
        if (!in_array($filter, $this->_filters)) {
            $this->_filters[] = $filter;
        }

        return $this;
    }

    /**
     * Add multiple filters by name.
     *
     * @param \Titon\Route\FilterList $filters
     * @return $this
     */
    public function addFilters(FilterList $filters): this {
        foreach ($filters as $filter) {
            $this->addFilter($filter);
        }

        return $this;
    }

    /**
     * Add an HTTP method to match against.
     *
     * @param string $method
     * @return $this
     */
    public function addMethod(string $method): this {
        if (!in_array($method, $this->_methods)) {
            $this->_methods[] = $method;
        }

        return $this;
    }

    /**
     * Add multiple HTTP methods to match against.
     *
     * @param \Titon\Route\MethodList $methods
     * @return $this
     */
    public function addMethods(MethodList $methods): this {
        foreach ($methods as $method) {
            $this->addMethod($method);
        }

        return $this;
    }

    /**
     * Add a regex pattern by token name.
     *
     * @param string $pattern
     * @param string $regex
     * @return $this
     */
    public function addPattern(string $pattern, string $regex): this {
        $this->_patterns[$pattern] = $regex;

        return $this;
    }

    /**
     * Add multiple regex patterns.
     *
     * @param \Titon\Route\PatternMap $patterns
     * @return $this
     */
    public function addPatterns(PatternMap $patterns): this {
        foreach ($patterns as $pattern => $regex) {
            $this->addPattern($pattern, $regex);
        }

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
            $matches = [];

            // Match regex pattern tokens first
            preg_match_all('/(\<)([^\<\>]+)(\>)/i', $path, $matches, PREG_SET_ORDER);
            $tokens = array_merge($tokens, $matches);

            // Then match regular tokens
            preg_match_all('/(\{|\(|\[)([a-z0-9\?]+)(\}|\)|\])/i', $path, $matches, PREG_SET_ORDER);
            $tokens = array_merge($tokens, $matches);

            if ($tokens) {
                foreach ($tokens as $match) {
                    $chunk = $match[0];
                    $open = array_key_exists(1, $match) ? $match[1] : ''; // opening brace
                    $token = array_key_exists(2, $match) ? $match[2] : ''; // token
                    $close = array_key_exists(3, $match) ? $match[3] : ''; // closing brace
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

                    } else if ($open === '<' && $close === '>' && $patterns->contains($token)) {
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

                    $this->_tokens[] = shape('token' => $token, 'optional' => $optional);
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
        return $this->_compiled = $compiled;
    }

    /**
     * Return the action to dispatch to.
     *
     * @return \Titon\Route\Action
     */
    public function getAction(): Action {
        return $this->_action;
    }

    /**
     * Return all filters.
     *
     * @return \Titon\Route\FilterList
     */
    public function getFilters(): FilterList {
        return $this->_filters;
    }

    /**
     * Return the HTTP method.
     *
     * @return \Titon\Route\MethodList
     */
    public function getMethods(): MethodList {
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
     * @param string $key
     * @return mixed
     */
    public function getParam(string $key): mixed {
        return $this->_params->get($key);
    }

    /**
     * Return all params.
     *
     * @return \Titon\Route\ParamMap
     */
    public function getParams(): ParamMap {
        return $this->_params;
    }

    /**
     * Return all the patterns used for compiling.
     *
     * @return \Titon\Route\PatternMap
     */
    public function getPatterns(): PatternMap {
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
     * @return \Titon\Route\TokenList
     */
    public function getTokens(): TokenList {
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
        $matches = [];

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
        $method = $this->getMethods()->map(fun('strtolower'));

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
            array_key_exists('HTTPS', $_SERVER) && $_SERVER['HTTPS'] === 'on' ||
            array_key_exists('SERVER_PORT', $_SERVER) && $_SERVER['SERVER_PORT'] == 443
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
     * Receive a list of matched values and apply it to the current route.
     * These matches will equate to tokens, arguments, and other required values.
     *
     * @param Vector<string> $matches
     * @return $this
     */
    public function match(Vector<string> $matches): this {
        $matches = $matches->toArray();
        $tokens = $this->getTokens();

        $this->_url = array_shift($matches);

        if ($matches && $tokens) {
            foreach ($tokens as $token) {
                $this->_params[$token['token']] = array_shift($matches);
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
    public function prepend(string $path): this {
        $this->_path = '/' . trim($path, '/') . rtrim($this->_path, '/');

        return $this;
    }

    /**
     * Set the list of filters to process.
     *
     * @param \Titon\Route\FilterList $filters
     * @return $this
     */
    public function setFilters(FilterList $filters): this {
        $this->_filters = $filters;

        return $this;
    }

    /**
     * Set the list of HTTP methods to match against.
     *
     * @param \Titon\Route\MethodList $methods
     * @return $this
     */
    public function setMethods(MethodList $methods): this {
        $this->_methods = $methods;

        return $this;
    }

    /**
     * Set a mapping of regex patterns to parse URLs with.
     *
     * @param \Titon\Route\PatternMap $patterns
     * @return $this
     */
    public function setPatterns(PatternMap $patterns): this {
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
