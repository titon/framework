<?php
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\View;

use Titon\Cache\Storage;

/**
 * The View acts as the hub between the templates and the rendering engine.
 * It keeps a mapping of template locations, helpers, data variables and more.
 *
 * @package Titon\View
 * @events
 *      view.preRender(View $view, $template)
 *      view.postRender(View $view, $response)
 *      view.preLocate($template, $type, array $paths)
 *      view.postLocate($template, $type, array $paths)
 */
interface View {

    /**
     * Constants for all the possible types of templates.
     */
    const TEMPLATE = 1;
    const LAYOUT = 2;
    const WRAPPER = 3;
    const PARTIAL = 4;
    const PRIVATE_TEMPLATE = 5;

    /**
     * Add a view helper.
     *
     * @param string $key
     * @param \Titon\View\Helper $helper
     * @return $this
     */
    public function addHelper($key, Helper $helper);

    /**
     * Add a template lookup path.
     *
     * @param string $path
     * @return $this
     */
    public function addPath($path);

    /**
     * Add multiple template lookup paths.
     *
     * @param array $paths
     * @return $this
     */
    public function addPaths(array $paths);

    /**
     * Format the current array of template parts. Any non-string values should be filtered.
     * Extensions should be appended on the end of the path.
     *
     * @param string $template
     * @return string
     */
    public function formatPath($template);

    /**
     * Return a helper by key.
     *
     * @param string $key
     * @return \Titon\View\Helper
     */
    public function getHelper($key);

    /**
     * Return all helpers.
     *
     * @return \Titon\View\Helper[]
     */
    public function getHelpers();

    /**
     * Return all paths.
     *
     * @return array
     */
    public function getPaths();

    /**
     * Return the storage engine.
     *
     * @return \Titon\Cache\Storage
     */
    public function getStorage();

    /**
     * Return a variable by key.
     *
     * @param string $key
     * @return mixed
     */
    public function getVariable($key);

    /**
     * Return all variables.
     *
     * @return array
     */
    public function getVariables();

    /**
     * Locate a template within the lookup paths.
     *
     * @param array|string $template
     * @param int $type
     * @return string
     */
    public function locateTemplate($template, $type = self::TEMPLATE);

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
    public function render($template, $private = false);

    /**
     * Render a single template and pass in optional variables.
     *
     * @param string $path
     * @param array $variables
     * @return string
     */
    public function renderTemplate($path, array $variables = []);

    /**
     * Set the storage engine to cache views.
     *
     * @param \Titon\Cache\Storage $storage
     * @return $this
     */
    public function setStorage(Storage $storage);

    /**
     * Set a view variable.
     *
     * @param string $key
     * @param mixed $value
     * @return $this
     */
    public function setVariable($key, $value);

    /**
     * Set multiple view variables.
     *
     * @param array $data
     * @return $this
     */
    public function setVariables(array $data);

}