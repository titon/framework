<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Environment;

/**
 * TODO
 *
 * @package Titon\Environment
 */
class Loader {

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

        foreach (parse_ini_file($path) as $key => $value) {
            $this->addVariable($key, $value);
        }

        $this->interpolateVariables();
    }

    /**
     * Add a loaded variable by formatting the key and value.
     *
     * @param string $key
     * @param string $value
     * @return $this
     */
    public function addVariable(string $key, string $value): this {
        $key = preg_replace('/[^A-Z0-9_]+/', '', strtoupper($key));

        $this->variables[$key] = $value;

        putenv("$key=$value");

        return $this;
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
            $this->variables[$key] = preg_replace_callback('/\{([A-Z0-9_]+)\}/g', ($matches) => $vars->get($matches[1]) ?: '', $value);
        }

        return $this;
    }

}
