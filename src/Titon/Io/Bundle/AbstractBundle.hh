<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Io\Bundle;

use Titon\Common\Cacheable;
use Titon\Io\Bundle;
use Titon\Io\DomainList;
use Titon\Io\DomainPathMap;
use Titon\Io\Folder;
use Titon\Io\PathList;
use Titon\Io\ReaderMap;
use Titon\Io\Reader;
use Titon\Io\Exception\MissingDomainException;
use Titon\Io\Exception\MissingReaderException;
use Titon\Utility\Inflector;
use Titon\Utility\Path;

/**
 * Abstract class that handles the loading of Readers and file locations.
 * The bundle can then search for a resource by name by cycling through each location
 * and parsing out the files contents.
 *
 * @package Titon\Io\Bundle
 */
abstract class AbstractBundle implements Bundle {
    use Cacheable;

    /**
     * Resource locations.
     *
     * @var \Titon\Io\DomainPathMap
     */
    protected DomainPathMap $_paths = Map {};

    /**
     * File readers.
     *
     * @var \Titon\Io\ReaderMap
     */
    protected ReaderMap $_readers = Map {};

    /**
     * {@inheritdoc}
     *
     * @uses Titon\Utility\Path
     */
    public function addPath(string $domain, string $path): this {
        if (!$this->getPaths()->contains($domain)) {
            $this->_paths[$domain] = Vector {};
        }

        $this->_paths[$domain][] = Path::ds($path, true);

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
        $type = Path::className(get_class($reader));
        $type = str_replace('reader', '', strtolower($type));

        $this->_readers[$type] = $reader;

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function getContents(string $domain): PathList {
        return $this->cache([__METHOD__, $domain], function() use ($domain) {
            $contents = Vector {};

            foreach ($this->getDomainPaths($domain) as $path) {
                $folder = new Folder($path);

                foreach ($folder->read() as $file) {
                    if ($file->isFile()) {
                        $contents[] = $file->path();
                    }
                }
            }

            return $contents;
        });
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
            return $this->_paths[$domain];
        }

        throw new MissingDomainException(sprintf('Domain %s does not exist', $domain));
    }

    /**
     * {@inheritdoc}
     */
    public function getPaths(): DomainPathMap {
        return $this->_paths;
    }

    /**
     * {@inheritdoc}
     */
    public function getReaders(): ReaderMap {
        return $this->_readers;
    }

    /**
     * {@inheritdoc}
     *
     * @uses Titon\Utility\Inflector
     *
     * @throws \Titon\Io\Exception\MissingReaderException
     */
    public function loadResource(string $domain, string $resource): ResourceMap {
        if ($this->getReaders()->isEmpty()) {
            throw new MissingReaderException('A Reader must be loaded to read Bundle resources');

        } else if ($cache = $this->getCache([__METHOD__, $resource])) {
            return $cache;
        }

        $contents = Map {};

        foreach ($this->getDomainPaths($domain) as $path) {
            foreach ($this->getReaders() as $ext => $reader) {
                $path = $path . Inflector::fileName($resource, $ext, false);

                if (file_exists($path)) {
                    if ($data = $reader->reset($path)->read()) {
                        $contents = Col::merge($contents, $data);
                    }
                }
            }
        }

        return $this->setCache([__METHOD__, $resource], $contents);
    }

}
