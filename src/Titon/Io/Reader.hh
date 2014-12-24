<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Io;

use Titon\Io\ResourceMap;

/**
 * Interface for reading file resources.
 *
 * @package Titon\Io
 */
interface Reader {

    /**
     * Return the current path.
     *
     * @return string
     */
    public function getPath(): string;

    /**
     * Return the extension for the type of reader.
     *
     * @return string
     */
    public function getResourceExt(): string;

    /**
     * Read the contents of the resource file.
     *
     * @return \Titon\Io\ResourceMap
     */
    public function readResource(): ResourceMap;

    /**
     * Reset the file path.
     *
     * @param string $path
     * @return $this
     */
    public function reset(string $path = ''): this;

}