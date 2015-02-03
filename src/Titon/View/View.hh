<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\View;

use Titon\Common\DataMap;
use Titon\Cache\Storage;

/**
 * The View class acts as the hub between the individual templates and the rendering engine.
 * It manages a list of lookup paths, which it uses to locate templates for rendering.
 *
 * @package Titon\View
 * @events
 *      view.rendering(View $view, string $template)
 *      view.rendered(View $view, string $response)
 *      view.locating(string $template, Template $type, Vector<string> $paths)
 *      view.located(string $path, Template $type)
 */
interface View {

    /**
     * Add a view helper.
     *
     * @param string $key
     * @param \Titon\View\Helper $helper
     * @return $this
     */
    public function addHelper(string $key, Helper $helper): this;

    /**
     * Add a lookup path.
     *
     * @param string $path
     * @return $this
     */
    public function addPath(string $path): this;

    /**
     * Add multiple lookup paths.
     *
     * @param \Titon\View\PathList $paths
     * @return $this
     */
    public function addPaths(PathList $paths): this;

    /**
     * Format the current template path by converting slashes and removing extensions.
     *
     * @param string $template
     * @return string
     */
    public function formatPath(string $template): string;

    /**
     * Return the template file extension.
     *
     * @return string
     */
    public function getExtension(): string;

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
     * Return all lookup paths.
     *
     * @return \Titon\View\PathList
     */
    public function getPaths(): PathList;

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
     * @return \Titon\Common\DataMap
     */
    public function getVariables(): DataMap;

    /**
     * Locate a template within the lookup paths and organize based on the type of template.
     *
     * @param string $template
     * @param \Titon\View\Template $type
     * @return string
     */
    public function locateTemplate(string $template, Template $type = Template::OPEN): string;

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
     * @param \Titon\Common\DataMap $variables
     * @return string
     */
    public function renderTemplate(string $path, DataMap $variables = Map {}): string;

    /**
     * Set the template file extension.
     *
     * @param string $ext
     * @return $this
     */
    public function setExtension(string $ext): this;

    /**
     * Set a list of lookup paths and overwrite any previously defined paths.
     *
     * @param \Titon\View\PathList $paths
     * @return $this
     */
    public function setPaths(PathList $paths): this;

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
     * @param \Titon\Common\DataMap $data
     * @return $this
     */
    public function setVariables(DataMap $data): this;

}
