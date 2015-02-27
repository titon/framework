<?hh
namespace Titon\Cache\Storage;

class ApcStorageTest extends AbstractStorageTest {

    protected function setUp(): void {
        if (!extension_loaded('apc')) {
            $this->markTestSkipped('APC is not installed or configured properly');
        }

        $this->object = new ApcStorage('apc-');

        parent::setUp();
    }

}
