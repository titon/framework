<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Io;

type DomainList = Vector<string>;
type DomainPathMap = Map<string, PathList>;
type PathList = Vector<string>;
type ReaderMap = Map<string, Reader>;
type WriterMap = Map<string, Writer>;

/**
 * Interface for the resource bundles library.
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
     * @param \Titon\Io\PathList $paths
     * @return $this
     */
    public function addPaths(string $domain, PathList $paths): this;

    /**
     * Add a file reader to use for resource parsing.
     *
     * @param \Titon\Io\Reader $reader
     * @return $this
     */
    public function addReader(Reader $reader): this;

    /**
     * Return a list of all files within a specific domain.
     *
     * @param string $domain
     * @return \Titon\Io\PathList
     */
    public function getContents(string $domain): PathList;

    /**
     * Return a list of all domain keys.
     *
     * @return \Titon\Io\DomainList
     */
    public function getDomains(): DomainList;

    /**
     * Return all resource paths for a single domain.
     *
     * @param string $domain
     * @return \Titon\Io\PathList
     */
    public function getDomainPaths(string $domain): PathList;

    /**
     * Return all resource paths for all domains.
     *
     * @return \Titon\Io\DomainPathMap
     */
    public function getPaths(): DomainPathMap;

    /**
     * Return all loaded Readers.
     *
     * @return \Titon\Io\ReaderMap
     */
    public function getReaders(): ReaderMap;

    /**
     * Parse the contents of every file that matches the resource name.
     * Begin by looping through all resource locations and all Readers.
     * If a resource is found that matches the name and a loaded Reader extension,
     * parse the file and merge its contents with the previous resource of the same name.
     *
     * @param string $domain
     * @param string $resource
     * @return \Titon\Io\ResourceMap
     */
    public function loadResource(string $domain, string $resource): ResourceMap;

}
