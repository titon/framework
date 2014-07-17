<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Test;

use Titon\Db\Query;
use \Exception;

/**
 * Allows fixtures to setup database records through the db layer.
 */
class Fixture {

    /**
     * Fully qualified repository class to use.
     *
     * @type string
     */
    protected $repository;

    /**
     * List of records to insert into the table.
     *
     * @type array
     */
    protected $records = [];

    /**
     * Repository instance.
     *
     * @type \Titon\Db\Repository
     */
    protected $_repository;

    /**
     * Create the database table using the table's schema.
     *
     * @return bool
     * @throws \Exception
     */
    public function createTable() {
        if (!$this->loadRepository()->createTable()) {
            throw new Exception(sprintf('Failed to create database table for %s', get_class($this)));
        }

        return true;
    }

    /**
     * Drop the table.
     *
     * @return bool
     */
    public function dropTable() {
        return (bool) $this->loadRepository()->dropTable();
    }

    /**
     * Instantiate a new table instance.
     *
     * @return \Titon\Db\Repository
     * @throws \Exception
     */
    public function loadRepository() {
        if ($this->_repository) {
            return $this->_repository;
        }

        if (!$this->repository) {
            throw new Exception(sprintf('Repository for %s has not been defined', get_class($this)));
        }

        $name = $this->repository;

        return $this->_repository = new $name();
    }

    /**
     * Insert records into the database.
     *
     * @return bool
     */
    public function insertRecords() {
        $this->loadRepository()->createMany($this->records);

        return true;
    }

    /**
     * Truncate the table of all records.
     *
     * @return bool
     */
    public function truncateTable() {
        return (bool) $this->loadRepository()->truncate();
    }

}