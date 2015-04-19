<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\View;

use Titon\Cache\Storage;
use Titon\Event\EmitsEvents;
use Titon\Event\Listener;
use Titon\Event\Subject;
use Titon\Utility\Inflector;
use Titon\Utility\Registry;
use Titon\View\Exception\MissingHelperException;
use Titon\View\Helper;

/**
 * Defines shared functionality for view managers.
 *
 * @package Titon\View\View
 */
abstract class AbstractView implements View, Subject {
    use EmitsEvents;

    /**
     * Variable data for templates.
     *
     * @var \Titon\View\DataMap
     */
    protected DataMap $data = Map {};

    /**
     * List of helpers.
     *
     * @var \Titon\View\HelperMap
     */
    protected HelperMap $helpers = Map {};

    /**
     * Template locator instance.
     *
     * @var \Titon\View\Locator
     */
    protected Locator $locator;

    /**
     * Storage engine.
     *
     * @var \Titon\Cache\Storage
     */
    protected ?Storage $storage;

    /**
     * Set the template locator through the constructor.
     *
     * @param \Titon\View\Locator $locator
     */
    public function __construct(Locator $locator) {
        $this->locator = $locator;
    }

    /**
     * {@inheritdoc}
     */
    public function getHelper(string $key): Helper {
        if ($this->helpers->contains($key)) {
            return $this->helpers[$key];
        }

        throw new MissingHelperException(sprintf('Helper %s does not exist', $key));
    }

    /**
     * {@inheritdoc}
     */
    public function getHelpers(): HelperMap {
        return $this->helpers;
    }

    /**
     * {@inheritdoc}
     */
    public function getLocator(): Locator {
        return $this->locator;
    }

    /**
     * Return the storage engine.
     *
     * @return \Titon\Cache\Storage
     */
    public function getStorage(): ?Storage {
        return $this->storage;
    }

    /**
     * {@inheritdoc}
     */
    public function getVariable(string $key): mixed {
        return $this->getVariables()->get($key);
    }

    /**
     * {@inheritdoc}
     */
    public function getVariables(): DataMap {
        return $this->data;
    }

    /**
     * {@inheritdoc}
     */
    public function setHelper(string $key, Helper $helper): this {
        $helper->setView($this);

        $this->helpers[$key] = $helper;

        if ($helper instanceof Listener) {
            $this->on($helper);
        }

        $this->setVariable($key, $helper);

        // Store so helpers can use helpers
        Registry::set($helper);

        return $this;
    }

    /**
     * Set the storage engine to cache views.
     *
     * @param \Titon\Cache\Storage $storage
     * @return $this
     */
    public function setStorage(Storage $storage): this {
        $this->storage = $storage;

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function setVariable(string $key, mixed $value): this {
        $this->data[Inflector::variable($key)] = $value;

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function setVariables(DataMap $data): this {
        foreach ($data as $key => $value) {
            $this->setVariable($key, $value);
        }

        return $this;
    }

}
