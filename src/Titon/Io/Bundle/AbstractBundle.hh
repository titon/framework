<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Io\Bundle;

use Titon\Io\Bundle;
use Titon\Io\DomainList;
use Titon\Io\DomainPathMap;
use Titon\Io\File;
use Titon\Io\Folder;
use Titon\Io\PathList;
use Titon\Io\Reader;
use Titon\Io\ReaderMap;
use Titon\Io\ResourceMap;
use Titon\Io\Exception\MissingDomainException;
use Titon\Io\Exception\MissingReaderException;
use Titon\Utility\Col;
use Titon\Utility\Inflector;
use Titon\Utility\Path;

/**
 * Abstract class that handles the loading of Readers and managing of lookup paths.
 * The bundle can then search for a resource by name by cycling through each lookup path
 * and parsing out the files contents.
 *
 * @package Titon\Io\Bundle
 */
abstract class AbstractBundle implements Bundle {

    /**
     * Resource locations.
     *
     * @var \Titon\Io\DomainPathMap
     */
    protected DomainPathMap $paths = Map {};

    /**
     * File readers.
     *
     * @var \Titon\Io\ReaderMap
     */
    protected ReaderMap $readers = Map {};

    /**
     * {@inheritdoc}
     *
     * @uses Titon\Utility\Path
     */
    public function addPath(string $domain, string $path): this {
        if (!$this->getPaths()->contains($domain)) {
            $this->paths[$domain] = Set {};
        }

        $this->paths[$domain][] = Path::ds($path, true);

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function addPaths(string $domain, PathList $paths): this {
        foreach ($paths as $path) {
            $this->addPath($domain, $path);
        }

        return $this;
    }

    /**
     * {@inheritdoc}
     *
     * @uses Titon\Utility\Path
     */
    public function addReader(Reader $reader): this {
        $this->readers[$reader->getResourceExt()] = $reader;

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function getContents(string $domain): PathList {
        $contents = Set {};

        foreach ($this->getDomainPaths($domain) as $path) {
            foreach ((new Folder($path))->files() as $file) {
                $contents[] = $file->path();
            }
        }

        return $contents;
    }

    /**
     * {@inheritdoc}
     */
    public function getDomains(): DomainList {
        return $this->getPaths()->keys();
    }

    /**
     * {@inheritdoc}
     *
     * @throws \Titon\Io\Exception\MissingDomainException
     */
    public function getDomainPaths(string $domain): PathList {
        if ($this->getPaths()->contains($domain)) {
            return $this->paths[$domain];
        }

        throw new MissingDomainException(sprintf('Domain %s does not exist', $domain));
    }

    /**
     * {@inheritdoc}
     */
    public function getPaths(): DomainPathMap {
        return $this->paths;
    }

    /**
     * {@inheritdoc}
     */
    public function getReaders(): ReaderMap {
        return $this->readers;
    }

    /**
     * {@inheritdoc}
     *
     * @uses Titon\Utility\Col
     * @uses Titon\Utility\Inflector
     *
     * @throws \Titon\Io\Exception\MissingReaderException
     */
    public function loadResource(string $domain, string $resource): ResourceMap {
        if ($this->getReaders()->isEmpty()) {
            throw new MissingReaderException('A Reader must be loaded to read bundle resources');
        }

        $contents = Map {};
        $readers = $this->getReaders();

        foreach ($this->getDomainPaths($domain) as $path) {
            foreach ($readers as $ext => $reader) {
                $resourcePath = $path . Inflector::fileName($resource, $ext);

                if (file_exists($resourcePath)) {
                    $contents = Col::merge($contents, $reader->reset($resourcePath)->readResource());
                }
            }
        }

        return $contents;
    }

}
