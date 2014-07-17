<?hh // strict
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\View\View;

use Titon\View\View;

/**
 * Interface for the rendering engine.
 *
 * @package Titon\View\View
 */
interface Engine {

    /**
     * Return a variable by key.
     *
     * @param string $key
     * @param mixed $default
     * @return mixed
     */
    public function data($key, $default = null);

    /**
     * Return the currently parsed template content.
     *
     * @return string
     */
    public function getContent();

    /**
     * Return the current layout.
     *
     * @return string
     */
    public function getLayout();

    /**
     * Return the list of wrappers.
     *
     * @return array
     */
    public function getWrapper();

    /**
     * Return the view instance.
     *
     * @return \Titon\View\View
     */
    public function getView();

    /**
     * Render a partial template at the defined path.
     * Optionally can pass an array of custom variables.
     *
     * @param string $partial
     * @param array $variables
     * @return string
     */
    public function open($partial, array $variables = []);

    /**
     * Render a template at the defined path.
     * Optionally can pass an array of custom variables.
     *
     * @param string $path
     * @param array $variables
     * @return string
     */
    public function render($path, array $variables = []);

    /**
     * Set the content.
     *
     * @param string $content
     * @return $this
     */
    public function setContent($content);

    /**
     * Set the parent view layer.
     *
     * @param \Titon\View\View $view
     * @return $this
     */
    public function setView(View $view);

    /**
     * Set the layout to use.
     *
     * @param string $name
     * @return $this
     */
    public function useLayout($name);

    /**
     * Set the wrappers to use.
     *
     * @param string|array $name
     * @return $this
     */
    public function wrapWith($name);

}
