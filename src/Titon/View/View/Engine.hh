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
    public function data(string $key, mixed $default = null): mixed;

    /**
     * Return the currently parsed template content.
     *
     * @return string
     */
    public function getContent(): string;

    /**
     * Return the current layout.
     *
     * @return string
     */
    public function getLayout(): string;

    /**
     * Return the list of wrappers.
     *
     * @return Vector<string>
     */
    public function getWrapper(): Vector<string>;

    /**
     * Return the view instance.
     *
     * @return $this
     */
    public function getView(): View;

    /**
     * Render a partial template at the defined path.
     * Optionally can pass an array of custom variables.
     *
     * @param string $partial
     * @param Map<string, mixed> $variables
     * @return string
     */
    public function open(string $partial, Map<string, mixed> $variables = Map {}): string;

    /**
     * Render a template at the defined path.
     * Optionally can pass an array of custom variables.
     *
     * @param string $path
     * @param Map<string, mixed> $variables
     * @return string
     */
    public function render(string $path, Map<string, mixed> $variables = Map {}): string;

    /**
     * Set the content.
     *
     * @param string $content
     * @return $this
     */
    public function setContent(string $content): this;

    /**
     * Set the parent view layer.
     *
     * @param \Titon\View\View $view
     * @return $this
     */
    public function setView(View $view): this;

    /**
     * Set the layout to use.
     *
     * @param string $name
     * @return $this
     */
    public function useLayout(string $name): this;

    /**
     * Set the wrappers to use.
     *
     * @param string $names
     * @return $this
     */
    public function wrapWith(...$names): this; // todo - type hint once variadic supports it

}
