<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\View;

use Titon\Common\DataMap;
use Titon\View\Engine\WrapperList;

/**
 * Interface for the rendering engine.
 *
 * @package Titon\View
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
     * Return the currently parsed template.
     *
     * @return string
     */
    public function getContent(): string;

    /**
     * Return the layout.
     *
     * @return string
     */
    public function getLayout(): string;

    /**
     * Return the list of wrappers.
     *
     * @return \Titon\View\WrapperList
     */
    public function getWrappers(): WrapperList;

    /**
     * Return the view instance.
     *
     * @return $this
     */
    public function getView(): ?View;

    /**
     * Render a template partial (nested templates) that is included within the current template.
     * Can optionally pass in a list of variables that is accessible in the template.
     *
     * @param string $partial
     * @param \Titon\Common\DataMap $variables
     * @return string
     */
    public function open(string $partial, DataMap $variables = Map {}): string;

    /**
     * Render a template at the defined absolute path.
     * Can optionally pass in a list of variables that is accessible in the template.
     *
     * @param string $path
     * @param \Titon\Common\DataMap $variables
     * @return string
     */
    public function render(string $path, DataMap $variables = Map {}): string;

    /**
     * Set the content.
     *
     * @param string $content
     * @return $this
     */
    public function setContent(string $content): this;

    /**
     * Set the parent view manager.
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
