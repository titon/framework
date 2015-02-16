<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Cache\Storage;

use Titon\Db\Query;
use Titon\Db\Repository;
use Titon\Db\RepositoryAware;

/**
 * A storage engine that uses a database table for storing data.
 * Requires the Titon DB package for executing queries.
 *
 * @package Titon\Cache\Storage
 */
class DatabaseStorage extends AbstractStorage {
    use RepositoryAware;

    /**
     * Set a repository object.
     *
     * @param \Titon\Db\Repository $repository
     * @param string $prefix
     */
    public function __construct(Repository $repository, string $prefix = '') {
        $this->setRepository($repository);

        parent::__construct($prefix);
    }

    /**
     * Return a select query for finding a cache record.
     *
     * @param string $key
     * @return \Titon\Db\Query
     */
    public function find(string $key): Query {
        return $this->getRepository()->select()->where('key', $this->getPrefix() . $key);
    }

    /**
     * {@inheritdoc}
     */
    public function flush(): bool {
        return $this->getRepository()->truncate();
    }

    /**
     * {@inheritdoc}
     */
    public function get(string $key): mixed {
        if ($entity = $this->find($key)->first()) {
            if ($entity->expires_at < date('Y-m-d H:i:s')) {
                $this->getRepository()->delete($entity->id);
                return null;
            }

            return unserialize($entity->value);
        }

        return null;
    }

    /**
     * {@inheritdoc}
     */
    public function has(string $key): bool {
        return (bool) $this->find($key)->count();
    }

    /**
     * {@inheritdoc}
     */
    public function remove(string $key): bool {
        return (bool) $this->getRepository()->query(Query::DELETE)->where('key', $this->getPrefix() . $key)->save();
    }

    /**
     * {@inheritdoc}
     */
    public function set(string $key, mixed $value, mixed $expires = null): bool {
        $repo = $this->getRepository();

        if ($entity = $this->find($key)->first()) {
            return (bool) $repo->update($entity->id, [
                'value' => serialize($value)
            ]);

        } else {
            return (bool) $repo->create([
                'key' => $this->getPrefix() . $key,
                'value' => serialize($value),
                'expires_at' => $this->expires($expires)
            ]);
        }
    }

}
