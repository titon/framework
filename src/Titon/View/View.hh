<?hh // strict
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\View;

use Titon\Common\DataMap;
use Titon\Cache\Storage;

type HelperMap = Map<string, Helper>;
type PathList = Vector<string>;

/**
 * The View acts as the hub between the templates and the rendering engine.
 * It keeps a mapping of template locations, helpers, data variables and more.
 *
 * @package Titon\View
 * @events
 *      view.rendering(View $view, $template)
 *      view.rendered(View $view, $response)
 *      view.locating($template, $type, array $paths)
 *      view.located($template, $type, array $paths)
 */
interface View {

    /**
     * Constants for all the possible types of templates.
     */
    const int TEMPLATE = 1;
    const int LAYOUT = 2;
    const int WRAPPER = 3;
    const int PARTIAL = 4;
    const int PRIVATE_TEMPLATE = 5;

    /**
     * Add a view helper.
     *
     * @param string $key
     * @param \Titon\View\Helper $helper
     * @return $this
     */
    public function addHelper(string $key, Helper $helper): this;

    /**
     * Add a template lookup path.
     *
     * @param string $path
     * @return $this
     */
    public function addPath(string $path): this;

    /**
     * Add multiple template lookup paths.
     *
     * @param \Titon\View\PathList $paths
     * @return $this
     */
    public function addPaths(PathList $paths): this;

    /**
     * Format the current array of template parts. Any non-string values should be filtered.
     * Extensions should be appended on the end of the path.
     *
     * @param mixed $template
     * @return string
     */
    public function formatPath(mixed $template): string;

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
     * Return all paths.
     *
     * @return \Titon\View\PathList
     */
    public function getPaths(): PathList;

    /**
     * Return the storage engine.
     *
     * @return \Titon\Cache\Storage
     */
    public function getStorage(): ?Storage;

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
     * Locate a template within the lookup paths.
     *
     * @param array|string $template
     * @param int $type
     * @return string
     */
    public function locateTemplate(mixed $template, int $type = self::TEMPLATE): string;

    /**
     * Determine the template path by parsing. the template argument.
     * Render the template and apply wrappers and layout.
     * Will render in order of template -> wrapper(s) -> layout.
     *
     * If $private is true, render a template from the private folder.
     *
     * @param string|array $template
     * @param bool $private
     * @return string
     */
    public function render(mixed $template, bool $private = false): string;

    /**
     * Render a single template and pass in optional variables.
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
     * Set the storage engine to cache views.
     *
     * @param \Titon\Cache\Storage $storage
     * @return $this
     */
    public function setStorage(Storage $storage): this;

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