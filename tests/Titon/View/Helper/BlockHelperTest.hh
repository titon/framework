<?hh
namespace Titon\View\Helper;

use Titon\Test\TestCase;

/**
 * @property \Titon\View\Helper\BlockHelper $object
 */
class BlockHelperTest extends TestCase {

    protected function setUp(): void {
        parent::setUp();

        $this->object = new BlockHelper();
    }

    public function testGetSetHas(): void {
        $this->assertFalse($this->object->has('block'));
        $this->assertEquals(null, $this->object->get('block'));

        $this->object->set('block', 'contents');

        $this->assertTrue($this->object->has('block'));
        $this->assertEquals('contents', $this->object->get('block'));
    }

    public function testAppendPrepend(): void {
        $this->object->set('block', 'contents');
        $this->assertEquals('contents', $this->object->get('block'));

        $this->object->append('block', '--after');
        $this->assertEquals('contents--after', $this->object->get('block'));

        $this->object->prepend('block', 'before--');
        $this->assertEquals('before--contents--after', $this->object->get('block'));

        // Create if doesn't exist
        $this->assertFalse($this->object->has('block2'));
        $this->object->append('block2', 'after');
        $this->assertTrue($this->object->has('block2'));

        $this->assertFalse($this->object->has('block3'));
        $this->object->prepend('block3', 'before');
        $this->assertTrue($this->object->has('block3'));
    }

    public function testCapture(): void {
        $this->assertEquals('', $this->object->get('block'));

        $this->object->start('block');
        echo 'foobar';
        $this->object->stop();

        $this->assertEquals('foobar', $this->object->get('block'));
    }

    public function testCaptureStateDetection(): void {
        $this->object->start('block');
        $this->assertEquals('block', $this->object->active());

        echo 'whatwhat';

        try {
            $this->object->start('block');
            $this->assertTrue(false);
        } catch (\Exception $e) {
            $this->assertTrue(true);
        }

        $this->object->stop();

        $this->assertEquals('whatwhat', $this->object->get('block'));
    }

    public function testNestedCapture(): void {
        $this->object->start('first');
        echo 1;
        $this->object->start('second');
        echo 2;
        $this->object->stop();
        echo 1;
        $this->object->stop();

        $this->assertEquals('11', $this->object->get('first'));
        $this->assertEquals('2', $this->object->get('second'));
    }

    public function testParentCapture(): void {
        $this->object->start('block');
        echo 'PARENT';
        $this->object->stop();

        $this->object->start('block');
        echo 'CHILD ' . $this->object->parent() . ' CHILD';
        $this->object->stop();

        $this->assertEquals('CHILD PARENT CHILD', $this->object->get('block'));
    }

    public function testCaptureShow(): void {
        $this->object->start('block');
        echo 'foobar';
        $contents = $this->object->show();

        $this->assertEquals('foobar', $contents);
    }

    public function testStop(): void {
        $this->object->start('block');
        echo 'foobar';
        $this->object->stop();
        $this->object->stop();

        $this->assertEquals('foobar', $this->object->get('block'));
    }

}
