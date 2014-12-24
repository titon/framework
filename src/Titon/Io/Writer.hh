<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Io;

use Titon\Io\ResourceMap;

/**
 * Interface for writing file resources.
 *
 * @package Titon\Io
 */
interface Writer {

    /**
     * Return the current path.
     *
     * @return string
     */
    public function getPath(): string;

    /**
     * Return the extension for the type of writer.
     *
     * @return string
     */
    public function getResourceExt(): string;

    /**
     * Reset the file path.
     *
     * @param string $path
     * @return $this
     */
    public function reset(string $path = ''): this;

    /**
     * Truncate the resource file and write data to it.
     *
     * @param \Titon\Io\ResourceMap $data
     * @return bool
     */
    public function writeResource(ResourceMap $data): bool;

}