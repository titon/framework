<?php
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Io\Bundle;

use Titon\Common\Base;
use Titon\Common\Cacheable;
use Titon\Io\Bundle;
use Titon\Io\Exception\MissingDomainException;
use Titon\Io\Reader;
use Titon\Io\Exception\MissingReaderException;
use Titon\Utility\Inflector;
use Titon\Utility\Path;
use Titon\Utility\Str;
use \DirectoryIterator;

/**
 * Abstract class that handles the loading of Readers and file locations.
 * The bundle can then search for a resource by name by cycling through each location
 * and parsing out the files contents.
 *
 * @package Titon\Io\Bundle
 */
abstract class AbstractBundle extends Base implements Bundle {
    use Cacheable;

    /**
     * Resource locations.
     *
     * @type Map<string, Vector<string>>
     */
    protected Map<string, Vector<string>> $_paths = Map {};

    /**
     * File readers.
     *
     * @type Map<string, Reader>
     */
    protected Map<string, Reader> $_readers = Map {};

    /**
     * {@inheritdoc}
     *
     * @uses Titon\Utility\Path
     */
    public function addPath(string $domain, string $path): this {
        if (!isset($this->_paths[$domain])) {
            $this->_paths[$domain] = Vector {};
        }

        $this->_paths[$domain][] = Path::ds($path, true);

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function addPaths(string $domain, Vector<string> $paths): this {
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
    public function getContents(string $domain): Vector<string> {
        return $this->cache([__METHOD__, $domain], function() use ($domain) {
            $contents = Vector {};

            foreach ($this->getDomainPaths($domain) as $path) {
                foreach (new DirectoryIterator($path) as $file) {
                    if ($file->isFile()) {
                        $contents[] = $file->getPathname();
                    }
                }
            }

            return $contents;
        });
    }

    /**
     * {@inheritdoc}
     */
    public function getDomains(): Vector<string> {
        return $this->_paths->keys();
    }

    /**
     * {@inheritdoc}
     *
     * @throws \Titon\Io\Exception\MissingDomainException
     */
    public function getDomainPaths(string $domain): Vector<string> {
        if (isset($this->_paths[$domain])) {
            return $this->_paths[$domain];
        }

        throw new MissingDomainException(sprintf('Domain %s does not exist', $domain));
    }

    /**
     * {@inheritdoc}
     */
    public function getPaths(): Map<string, Vector<string>> {
        return $this->_paths;
    }

    /**
     * {@inheritdoc}
     */
    public function getReaders(): Map<string, Reader> {
        return $this->_readers;
    }

    /**
     * {@inheritdoc}
     *
     * @uses Titon\Utility\Inflector
     * @uses Titon\Utility\Str
     *
     * @throws \Titon\Io\Exception\MissingReaderException
     */
    public function loadResource(string $domain, string $resource): Vector<mixed> {
        if ($this->_readers->isEmpty()) {
            throw new MissingReaderException('A Reader must be loaded to read Bundle resources');

        } else if ($cache = $this->getCache([__METHOD__, $resource])) {
            return $cache;
        }

        $contents = Vector {};
        $config = $this->allConfig();

        foreach ($this->getPaths($domain) as $path) {
            $path = Str::insert($path, $config);

            foreach ($this->getReaders() as $ext => $reader) {
                $path = $path . Inflector::fileName($resource, $ext, false);

                if (file_exists($path)) {
                    if ($data = $reader->reset($path)->read()) {
                        $contents->addAll($data);
                    }
                }
            }
        }

        return $this->setCache([__METHOD__, $resource], $contents);
    }

}
