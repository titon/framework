<?hh // strict
namespace Titon\Cache\Storage;

use Titon\Db\Query;
use Titon\Test\Stub\Repository\Cache;

class DatabaseStorageTest extends AbstractStorageTest {

    protected function setUp(): void {
        if (!class_exists('Titon\Db\Query')) {
            $this->markTestSkipped('Requires the model package');
        }

        $this->loadFixtures('Cache');

        $this->object = new DatabaseStorage(new Cache(), 'db-');

        parent::setUp();
    }

    protected function tearDown(): void {
        parent::tearDown();

        $this->unloadFixtures('Cache');
    }

}
