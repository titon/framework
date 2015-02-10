<?hh
/**
 * @author Alex Phillips <exonintrendo@gmail.com>
 * Date: 2/5/15
 * Time: 6:05 PM
 */

namespace Titon\Context;

use Titon\Test\TestCase;

/**
 * @property \Titon\Context\Depository $object
 */
class DepositoryTest extends TestCase
{
    protected function setUp()
    {
        parent::setUp();

        $this->object = new Depository();
    }

    public function testClosure()
    {
        $this->object->add('test', function() {
            $test = new \StdClass();
            $test->foo = 'bar';

            return $test;
        });

        $this->assertEquals('bar', $this->object->make('test')->foo);
    }

    public function testSingleton()
    {
        $test = new \StdClass();
        $test->foo = 'bar';
        $this->object->singleton('test', $test);

        $this->assertEquals('bar', $this->object->make('test')->foo);
    }
}