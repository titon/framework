<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Context\ServiceProvider;

use Titon\Context\Depository;
use Titon\Context\ServiceProvider;
use Titon\Context\ClassList;

/**
 * A Service Provider is an object that allows management for dependencies within
 * an application and register them with the Depository together. This also allows
 * for deferred loading of objects so they aren't created until needed.
 *
 * @package Titon\Context\ServiceProvider
 */
abstract class AbstractServiceProvider implements ServiceProvider {

    /**
     * Flag to determine if the service provided has already been initialized
     * so that the Depository doesn't attempt to register already resolved
     * items.
     *
     * @var bool
     */
    protected bool $initialized = false;

    /**
     * The Depository container used to register items in the service provider.
     *
     * @var \Titon\Context\Depository
     */
    protected ?Depository $depository;

    /**
     * An optional Vector collection containing the namespaced class names of
     * what the service provider actually provides. If this is left empty, then
     * the registered items will NOT be deferred and will be registered as the
     * service provider is registered with the Depository.
     *
     * @var \Titon\Context\ClassList
     */
    protected ClassList $provides = Vector {};

    /**
     * {@inheritdoc}
     */
    public function initialize(): void {
        if ($this->initialized === false) {
            $this->register();

            $this->initialized = true;
        }
    }

    /**
     * {@inheritdoc}
     */
    public function getProvides(): ClassList {
        return $this->provides;
    }

    /**
     * {@inheritdoc}
     */
    public function provides(string $className): bool {
        return $this->provides->linearSearch($className) >= 0;
    }

    /**
     * {@inheritdoc}
     */
    public function setDepository(Depository $depository): void {
        $this->depository = $depository;
    }
}