<?hh // strict
namespace Titon\Cache\Storage;

class MemoryStorageTest extends AbstractStorageTest {

    protected function setUp(): void {
        $this->object = new MemoryStorage('memory-');

        parent::setUp();
    }

}
