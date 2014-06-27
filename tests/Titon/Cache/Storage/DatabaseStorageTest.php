<?php
namespace Titon\Cache\Storage;

use Titon\Db\Query;
use Titon\Test\Stub\Repository\Cache;

class DatabaseStorageTest extends AbstractStorageTest {

    protected function setUp() {
        if (!class_exists('Titon\Db\Query')) {
            $this->markTestSkipped('Test skipped; Please install titon/db via Composer');
        }

        $this->loadFixtures('Cache');

        $this->object = new DatabaseStorage(new Cache());

        parent::setUp();
    }

    protected function tearDown() {
        parent::tearDown();

        $this->unloadFixtures('Cache');
    }

}