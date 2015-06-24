<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\View;

use Titon\Cache\Storage;

/**
 * The View class acts as the hub between the individual templates and the rendering engine.
 * It manages a list of lookup paths, which it uses to locate templates for rendering.
 *
 * @package Titon\View
 */
interface View {

    /**
     * Return a helper by key.
     *
     * @param string $key
     * @return \Titon\View\Helper
     */
    public function getHelper(string $key): Helper;

    /**
     * Return all helpers.
     *
     * @return \Titon\View\HelperMap
     */
    public function getHelpers(): HelperMap;

    /**
     * Return the template locator instance.
     *
     * @return \Titon\View\Locator
     */
    public function getLocator(): Locator;

    /**
     * Return a variable by key.
     *
     * @param string $key
     * @return mixed
     */
    public function getVariable(string $key): mixed;

    /**
     * Return all variables.
     *
     * @return \Titon\View\DataMap
     */
    public function getVariables(): DataMap;

    /**
     * The all-in-one rendering method that pieces together the layout, wrapper, and template,
     * and returns a single rendered response.
     *
     * If `$private` is true, render a template from the private folder.
     *
     * @param string $template
     * @param bool $private
     * @return string
     */
    public function render(string $template, bool $private = false): string;

    /**
     * Render a single template from the defined file system path.
     * Can optionally pass in a list of variables that is accessible in the template.
     *
     * @param string $path
     * @param \Titon\View\DataMap $variables
     * @return string
     */
    public function renderTemplate(string $path, DataMap $variables = Map {}): string;

    /**
     * Set a view helper.
     *
     * @param string $key
     * @param \Titon\View\Helper $helper
     * @return $this
     */
    public function setHelper(string $key, Helper $helper): this;

    /**
     * Set a view variable.
     *
     * @param string $key
     * @param mixed $value
     * @return $this
     */
    public function setVariable(string $key, mixed $value): this;

    /**
     * Set multiple view variables.
     *
     * @param \Titon\View\DataMap $data
     * @return $this
     */
    public function setVariables(DataMap $data): this;

}
