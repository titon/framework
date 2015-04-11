<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\View;

use Titon\View\Locator\PathList;

/**
 * The Locator provides an interface for locating a defined template within a list of lookup paths.
 *
 * @package Titon\View
 */
interface Locator {

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
     * Return the template file extension.
     *
     * @return string
     */
    public function getExtension(): string;

    /**
     * Return all lookup paths.
     *
     * @return \Titon\View\PathList
     */
    public function getPaths(): PathList;

    /**
     * Locate a template within the lookup paths and organize based on the type of template.
     *
     * @param string $template
     * @param \Titon\View\Template $type
     * @return string
     */
    public function locate(string $template, Template $type = Template::OPEN): string;

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

}
