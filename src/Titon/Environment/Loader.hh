<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Environment;

/**
 * The Loader handles the parsing and packaging of variables found in `.env` files.
 *
 * @package Titon\Environment
 */
class Loader {

    /**
     * Variables that should be immutable once loaded.
     *
     * @var \Titon\Environment\ImmutableList
     */
    protected static ImmutableList $immutable = Vector {'APP_ENV'};

    /**
     * Variables loaded from the defined `.env` file.
     *
     * @var \Titon\Environment\VariableMap
     */
    protected VariableMap $variables;

    /**
     * Load and format variables from the defined file.
     *
     * @param string $path
     * @param \Titon\Environment\VariableMap $variables
     */
    public function __construct(string $path, VariableMap $variables = Map {}) {
        $this->variables = $variables;
        $this->parseFile($path);
        $this->interpolateVariables();
    }

    /**
     * Add a loaded variable by formatting the key and value.
     *
     * @param string $key
     * @param string $value
     * @return $this
     */
    public function addVariable(string $key, mixed $value): this {
        $key = preg_replace('/[^A-Z0-9_]+/', '', strtoupper($key));

        // Don't overwrite immutable keys
        if (static::isImmutable($key) && $this->variables->contains($key)) {
            return $this;
        }

        // Box the value
        if (is_array($value)) {
            $value = new ImmMap($value);

        } else if (is_numeric($value)) {
            $value = (strpos($value, '.') !== false) ? (float) $value : (int) $value;

        } else if ($value === 'true' || $value === '1') {
            $value = true;

        } else if ($value === 'false' || $value === '') {
            $value = false;
        }

        $this->variables[$key] = $value;

        // Only non traversables should be set
        if (!$value instanceof ImmMap) {
            putenv($key . '=' . (string) $value);
        }

        return $this;
    }

    /**
     * Return the value of a variable defined by key.
     *
     * @param string $key
     * @return mixed
     */
    public function getVariable(string $key): mixed {
        return $this->getVariables()->get($key) ?: '';
    }

    /**
     * Return all loaded variables.
     *
     * @return \Titon\Environment\VariableMap
     */
    public function getVariables(): VariableMap {
        return $this->variables;
    }

    /**
     * Interpolate nested variables within each variable using the format of `{VAR}`.
     *
     * @return $this
     */
    public function interpolateVariables(): this {
        $vars = $this->variables;

        foreach ($vars as $key => $value) {
            if (is_string($value)) {
                $this->variables[$key] = preg_replace_callback('/\{([A-Z0-9_]+)\}/', ($matches) ==> $vars->get($matches[1]) ?: '', $value);
            }
        }

        return $this;
    }

    /**
     * Check if a variable is immutable by name.
     *
     * @param string $key
     * @return bool
     */
    public static function isImmutable(string $key): bool {
        return (static::$immutable->linearSearch($key) >= 0);
    }

    /**
     * Lock a variable and make it immutable.
     *
     * @param string $key
     */
    public static function lock(string $key): void {
        static::$immutable[] = $key;
    }

    /**
     * Parse the contents of the environment file by removing comments and extracting variables.
     *
     * @param string $path
     * @return $this
     */
    public function parseFile(string $path): this {
        $content = file_get_contents($path);

        // Remove custom comments
        $content = preg_replace('/(#|\/\/)[^\r\n]*/', '', $content);

        // Extract variables
        foreach (parse_ini_string($content) as $key => $value) {
            $this->addVariable($key, $value);
        }

        return $this;
    }

    /**
     * Unlock a variable and make it mutable.
     *
     * @param string $key
     */
    public static function unlock(string $key): void {
        $index = static::$immutable->linearSearch($key);

        if ($index >= 0) {
            static::$immutable->removeKey($index);
        }
    }

}
