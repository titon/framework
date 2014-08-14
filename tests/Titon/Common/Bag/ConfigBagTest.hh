<?hh
namespace Titon\Common\Bag;

use Titon\Test\TestCase;

/**
 * @property \Titon\Common\Bag\ConfigBag $object
 */
class ConfigBagTest extends TestCase {

    protected $defaults = Map {
        'boolean' => true,
        'integer' => 12345,
        'string' => 'foobar',
        'float' => 50.25,
        'map' => Map {
            'key' => 'value'
        },
        'vector' => Vector {1, 2, 3}
    };

    protected function setUp() {
        parent::setUp();

        $this->object = new ConfigBag(Map {}, $this->defaults);
    }

    public function testGetDefaults() {
        $this->object->set('boolean', false);

        $this->assertEquals($this->defaults, $this->object->getDefaults());
    }

}