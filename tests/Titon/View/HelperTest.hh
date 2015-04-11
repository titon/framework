<?hh
namespace Titon\View;

use Titon\Test\Stub\View\HelperStub;
use Titon\Test\Stub\View\LocatorStub;
use Titon\Test\Stub\View\ViewStub;
use Titon\Test\TestCase;
use Titon\View\Helper\TagMap;

/**
 * @property \Titon\View\Helper $object
 */
class HelperTest extends TestCase {

    protected function setUp(): void {
        parent::setUp();

        $this->object = new HelperStub();
    }

    public function testAttributes(): void {
        $this->assertEquals('', $this->object->attributes(Map {}));
        $this->assertEquals(' key="value"', $this->object->attributes(Map {'key' => 'value'}));
        $this->assertEquals(' foo="bar" key="value" number="1"', $this->object->attributes(Map {'key' => 'value', 'foo' => 'bar', 'number' => 1}));
        $this->assertEquals(' bool="1" key="value" null=""', $this->object->attributes(Map {'key' => 'value', 'null' => null, 'bool' => true}));

        // escaping
        $this->assertEquals(' double="&quot;quotes&quot;" key="value" single="&#039;quotes&#039;"', $this->object->attributes(Map {'key' => 'value', 'double' => '"quotes"', 'single' => "'quotes'"}));
        $this->assertEquals(' double=""quotes"" key="value" single="\'quotes\'"', $this->object->attributes(Map {'key' => 'value', 'double' => '"quotes"', 'single' => "'quotes'", 'escape' => false}));
        $this->assertEquals(' double=""quotes"" key="value" single="&#039;quotes&#039;"', $this->object->attributes(Map {'key' => 'value', 'double' => '"quotes"', 'single' => "'quotes'", 'escape' => ['double']}));
        $this->assertEquals(' double="&quot;quotes&quot;" key="value" single="\'quotes\'"', $this->object->attributes(Map {'key' => 'value', 'double' => '"quotes"', 'single' => "'quotes'", 'escape' => ['single']}));
        $this->assertEquals(' double=""quotes"" key="value" single="\'quotes\'"', $this->object->attributes(Map {'key' => 'value', 'double' => '"quotes"', 'single' => "'quotes'", 'escape' => ['single', 'double']}));

        // remove
        $this->assertEquals('', $this->object->attributes(Map {'key' => 'value'}, Vector {'key'}));
        $this->assertEquals(' foo="bar" number="1"', $this->object->attributes(Map {'key' => 'value', 'foo' => 'bar', 'number' => 1}, Vector {'key'}));
        $this->assertEquals(' bool="1"', $this->object->attributes(Map {'key' => 'value', 'null' => null, 'bool' => true}, Vector {'key', 'null'}));
    }

    public function testEscape(): void {
        $value = 'This is "double" and \'single\' quotes.';

        $this->assertEquals('This is &quot;double&quot; and &#039;single&#039; quotes.', $this->object->escape($value));
        $this->assertEquals('This is "double" and \'single\' quotes.', $this->object->escape($value, false));

        $this->object->setEscaping(false);

        $this->assertEquals('This is "double" and \'single\' quotes.', $this->object->escape($value));
        $this->assertEquals('This is &quot;double&quot; and &#039;single&#039; quotes.', $this->object->escape($value, true));

        $this->object->setEscaping(true);

        $this->assertEquals('This is &quot;double&quot; and &#039;single&#039; quotes.', $this->object->escape($value));
    }

    public function testGetSetEscaping(): void {
        $this->assertTrue($this->object->getEscaping());

        $this->object->setEscaping(false);

        $this->assertFalse($this->object->getEscaping());
    }

    public function testGetHelper(): void {
        $helper = new HelperStub();

        $view = new ViewStub(new LocatorStub('/'));
        $view->setHelper('html', $helper);

        $this->object->setView($view);

        $this->assertEquals($helper, $this->object->getHelper('html'));
    }

    /**
     * @expectedException \Titon\View\Exception\MissingHelperException
     */
    public function testGetHelperThrowsMissing(): void {
        $this->object->getHelper('html');
    }

    public function testGetTag(): void {
        $this->assertEquals('<tag>{body}</tag>', $this->object->getTag('noattr'));
    }

    /**
     * @expectedException \Titon\View\Exception\MissingTagException
     */
    public function testGetTagThrowsMissing(): void {
        $this->object->getTag('foobar');
    }

    public function testGetSetView(): void {
        $view = new ViewStub(new LocatorStub('/'));

        $this->assertEquals(null, $this->object->getView());

        $this->object->setView($view);

        $this->assertEquals($view, $this->object->getView());
    }

    public function testTag(): void {
        $this->assertEquals('<tag>{body}</tag>' . PHP_EOL, $this->object->tag('noattr'));
        $this->assertEquals('<tag>body</tag>' . PHP_EOL, $this->object->tag('noattr', Map {'fake' => 'attr', 'body' => 'body'}));
        $this->assertEquals('<tag value />' . PHP_EOL, $this->object->tag('nobody', Map {'attr' => ' value'}));
        $this->assertEquals('<tag 1 2>3</tag>' . PHP_EOL, $this->object->tag('custom', Map {'one' => 1, 'two' => 2, 'three' => 3}));
        $this->assertEquals('<tag 1>2</tag>3' . PHP_EOL, $this->object->tag('default', Map {'0' => 1, '1' => 2, '2' => 3}));
    }

}
