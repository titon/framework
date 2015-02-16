<?hh
namespace Titon\Cache\Storage;

class MemoryStorageTest extends AbstractStorageTest {

    protected function setUp() {
        $this->object = new MemoryStorage('memory-');

        parent::setUp();
    }

}
