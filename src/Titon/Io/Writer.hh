<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
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
    public function path(): string;

    /**
     * Reset the file path.
     *
     * @param string $path
     * @return $this
     */
    public function reset(string $path = ''): this;

    /**
     * Truncate source file and write data to it.
     *
     * @param \Titon\Io\ResourceMap $data
     * @return bool
     */
    public function write(ResourceMap $data): bool;

}