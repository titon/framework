<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Route;

use Titon\Route\Exception\MissingPatternException;
use Titon\Route\Exception\NoMatchException;
use Titon\Utility\Registry;
use \ReflectionFunctionAbstract;
use \ReflectionMethod;
use \Serializable;

type ConditionCallback = (function(Route): bool);
type ConditionList = Vector<ConditionCallback>;
type FilterList = Vector<string>;
type MethodList = Vector<string>;
type ParamMap = Map<string, mixed>;
type PatternMap = Map<string, string>;
type Token = shape('token' => string, 'optional' => bool);
type TokenList = Vector<Token>;

/**
 * Represents the skeleton for an individual route. A route is paired with a tokenized path that is matched and converted to a
 * routeable URL. Routes support filters, patterns, conditional matching, and more.
 *
 * @package Titon\Route
 */
class Route implements Serializable {

    /**
     * Pre-defined regex patterns.
     */
    const string ALPHA = '([a-z\_\-\+]+)';
    const string ALNUM = '([a-z0-9\_\-\+]+)';
    const string NUMERIC = '([0-9\.]+)';
    const string WILDCARD = '([^\/]+)';
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
     * List of conditions to validate against.
     *
     * @type \Titon\Route\ConditionList
     */
    protected ConditionList $_conditions = Vector {};

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
     * Store the tokenized URL to match and the action to route to.
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
     * Add a condition callback.
     *
     * @param \Titon\Route\ConditionCallback $condition
     * @return $this
     */
    public function addCondition(ConditionCallback $condition): this {
        $this->_conditions[] = $condition;

        return $this;
    }

    /**
     * Add multiple conditions.
     *
     * @param \Titon\Route\ConditionList $conditions
     * @return $this
     */
    public function addConditions(ConditionList $conditions): this {
        foreach ($conditions as $condition) {
            $this->addCondition($condition);
        }

        return $this;
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
            $this->_methods[] = strtolower($method);
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
                        $pattern = self::ALNUM;

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
     * Dispatch the current route to the defined action only if the route has been matched.
     * The dispatcher will use the params gathered from the token list to pass as arguments to the action.
     * Arguments will take into account default values defined on the method.
     *
     * @return mixed - The response of the action call
     * @exception \Titon\Route\Exception\NoMatchException
     */
    public function dispatch(): mixed {
        if (!$this->isMatched()) {
            throw new NoMatchException('Route cannot be dispatched unless it has been matched');
        }

        $action = $this->getAction();
        $object = Registry::factory($action['class']);
        $method = new ReflectionMethod($object, $action['action']);

        return $method->invokeArgs($object, $this->_getArguments($method));
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
     * Return the list of conditions.
     *
     * @return \Titon\Route\ConditionList
     */
    public function getConditions(): ConditionList {
        return $this->_conditions;
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

        // Compile the regex pattern
        $this->compile();

        // Match the route based on a set of conditions
        if (!$this->isMethod()) {
            return false;

        } else if (!$this->isSecure()) {
            return false;

        } else if (!$this->isValid()) {
            return false;

        } else if ($this->getPath() === $url) {
            return true;

        } else if (preg_match('~^' . $this->compile() . '$~i', $url, $matches)) {
            $this->match(new Vector($matches));

            return true;
        }

        return false;
    }

    /**
     * Return true if the route has been matched.
     *
     * @return bool
     */
    public function isMatched(): bool {
        return (bool) $this->url();
    }

    /**
     * Validates the route matches the correct HTTP method.
     *
     * @return bool
     */
    public function isMethod(): bool {
        $methods = $this->getMethods();

        if ($methods && !in_array(strtolower($_SERVER['REQUEST_METHOD']), $methods, true)) {
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
     * Validate the route is matchable by running through all defined conditions.
     *
     * @return bool
     */
    public function isValid(): bool {
        foreach ($this->getConditions() as $condition) {
            if (!call_user_func($condition, $this)) {
                return false;
            }
        }

        return true;
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
     * Serialize the compiled route for increasing performance when caching mapped routes.
     */
    public function serialize(): string {
        return serialize(Map {
            'action' => $this->_action,
            'compiled' => $this->_compiled,
            'filters' => $this->_filters,
            'methods' => $this->_methods,
            'patterns' => $this->_patterns,
            'path' => $this->_path,
            'secure' => $this->_secure,
            'static' => $this->_static,
            'tokens' => $this->_tokens
        });
    }

    /**
     * Set the action to dispatch to.
     *
     * @param \Titon\Route\Action $action
     * @return $this
     */
    public function setAction(Action $action): this {
        $this->_action = $action;

        return $this;
    }

    /**
     * Set the list of conditions to validate.
     *
     * @param \Titon\Route\ConditionList $conditions
     * @return $this
     */
    public function setConditions(ConditionList $conditions): this {
        $this->_conditions = $conditions;

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
        $this->_methods = $methods->map(fun('strtolower'));

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
     * Unserialize the route and set the internal values.
     *
     * @param mixed $data
     */
    public function unserialize($data): void { // TODO - type hint
        $data = unserialize($data);

        $this->_path = $data['path'];
        $this->_action = $data['action'];
        $this->_tokens = $data['tokens'];
        $this->_compiled = $data['compiled'];

        $this->setFilters($data['filters']);
        $this->setMethods($data['methods']);
        $this->setPatterns($data['patterns']);
        $this->setSecure($data['secure']);
        $this->setStatic($data['static']);
    }

    /**
     * Return the currently matched full URL.
     *
     * @return string
     */
    public function url(): string {
        return $this->_url;
    }

    /**
     * Gather a list of arguments to pass to the dispatcher based on the tokens and params from the route.
     * Loop through and set any default values using reflection.
     *
     * @param \ReflectionFunctionAbstract $method
     * @return Vector<mixed>
     */
    protected function _getArguments(ReflectionFunctionAbstract $method): Vector<mixed> {
        $tokens = $this->getTokens();
        $args = $this->getParams()->values();

        foreach ($method->getParameters() as $i => $param) {
            if (!$tokens->containsKey($i)) {
                continue;
            }

            if ($tokens[$i]['optional'] && (!array_key_exists($i, $args) || $args[$i] === '' || $args[$i] === null)) {
                $args[$i] = $param->getDefaultValue();
            }

            // Type cast the values to match the argument type hint
            switch ($param->getTypehintText()) {
                case 'HH\string': $args[$i] = (string) $args[$i]; break;
                case 'HH\bool': $args[$i] = (bool) $args[$i]; break;
                case 'HH\int': $args[$i] = (int) $args[$i]; break;
            }
        }

        return $args;
    }

}
