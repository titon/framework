<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Io;

use Titon\Io\Reader;

/**
 * Interface for the bundles library.
 *
 * @package Titon\Io
 */
interface Bundle {

    /**
     * Add a resource path to use during the lookup cycle.
     *
     * @param string $domain
     * @param string $path
     * @return $this
     */
    public function addPath(string $domain, string $path): this;

    /**
     * Add multiple resource paths.
     *
     * @param string $domain
     * @param Vector<string> $paths
     * @return $this
     */
    public function addPaths(string $domain, Vector<string> $paths): this;

    /**
     * Add a file reader to use for resource parsing.
     *
     * @param \Titon\Io\Reader $reader
     * @return $this
     */
    public function addReader(Reader $reader): this;

    /**
     * Return a list of all files within all resource locations.
     *
     * @param string $domain
     * @return array
     */
    public function getContents(string $domain): Vector<string>;

    /**
     * Return a list of all domain keys.
     *
     * @return Vector<string>
     */
    public function getDomains(): Vector<string>;

    /**
     * Return all resource paths for a single domain.
     *
     * @param string $domain
     * @return Vector<string>
     */
    public function getDomainPaths(string $domain): Vector<string>;

    /**
     * Return all resource paths for all domains.
     *
     * @return Map<string, Vector<string>>
     */
    public function getPaths(): Map<string, Vector<string>>;

    /**
     * Return all loaded Readers.
     *
     * @return Map<string, Reader>
     */
    public function getReaders(): Map<string, Reader>;

    /**
     * Parse the contents of every file that matches the resource name.
     * Begin by looping through all resource locations and all Readers.
     * If a resource is found that matches the name and a loaded Reader extension,
     * parse the file and merge its contents with the previous resource of the same name.
     *
     * @param string $domain
     * @param string $resource
     * @return Vector<mixed>
     */
    public function loadResource(string $domain, string $resource): Vector<mixed>;

}
