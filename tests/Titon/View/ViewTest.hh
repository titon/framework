<?hh
namespace Titon\View;

use Titon\Test\Stub\View\LocatorStub;
use Titon\Test\Stub\View\ViewStub;
use Titon\Test\TestCase;
use Titon\Utility\Registry;
use Titon\View\Helper\HtmlHelper;
use Titon\View\Helper\FormHelper;

/**
 * @property \Titon\View\View $object
 */
class ViewTest extends TestCase {

    protected function setUp(): void {
        parent::setUp();

        $this->vfs()->createStructure(static::generateViewStructure());

        $this->object = new ViewStub(new LocatorStub([
            $this->vfs()->path('/views'),
            $this->vfs()->path('/views-fallback')
        ]));
    }

    public static function generateViewStructure(): array<string, mixed> {
        return [
            '/views/' => [
                'private/' => [
                    'errors/' => [
                        '404.tpl' => 'private.errors.404'
                    ],
                    'layouts/' => [
                        'default.tpl' => '<layout><?= $this->getContent(); ?></layout>'
                    ],
                    'partials/' => [
                        'nested/' => [
                            'include.tpl' => 'private.partials.nested.include'
                        ],
                        'include.tpl' => 'private.partials.include',
                        'variables.tpl' => '<?= $name; ?> - <?= $type; ?> - <?= $filename; ?>'
                    ],
                    'wrappers/' => [
                        'wrapper.tpl' => '<wrapper><?= $this->getContent(); ?></wrapper>'
                    ],
                    'root.tpl' => 'private.root'
                ],
                'public/' => [
                    'index/' => [
                        'add.tpl' => 'public.index.add',
                        'edit.tpl' => 'public.index.edit',
                        'index.tpl' => 'public.index.index',
                        'include.tpl' => 'public.index.include - <?= $this->open(\'nested/include\'); ?>',
                        'view.tpl' => 'public.index.view',
                        'view.xml.tpl' => 'public.index.view.xml',
                    ],
                    'lang/' => [
                        'index.en-us.tpl' => 'public.lang.index.en-us',
                        'index.fr.tpl' => 'public.lang.index.fr',
                        'index.tpl' => 'public.lang.index',
                    ],
                    'root.tpl' => 'public.root'
                ]
            ],

            '/views-fallback' => [
                'private/' => [
                    'emails/' => [
                        'example.html.tpl' => "<!DOCTYPE html>\n<html>\n<body>\n    This is an example email template.<br>\n    It is an <b>HTML</b> specific <i>template</i>.<br>\n    - Titon\n</body>\n</html>",
                    ],
                    'layouts/' => [
                        'fallback.tpl' => '<fallbackLayout><?= $this->getContent(); ?></fallbackLayout>'
                    ],
                    'wrappers/' => [
                        'fallback.tpl' => '<fallbackWrapper><?= $this->getContent(); ?></fallbackWrapper>'
                    ]
                ]
            ]
        ];
    }

    public function testGetSetHelpers(): void {
        $form = new FormHelper();
        $html = new HtmlHelper();

        $this->assertEquals(Map {}, $this->object->getHelpers());

        $this->object->setHelper('html', $html);

        $this->assertEquals(Map {
            'html' => $html
        }, $this->object->getHelpers());

        $this->object->setHelper('form', $form);

        $this->assertEquals(Map {
            'html' => $html,
            'form' => $form
        }, $this->object->getHelpers());

        $this->assertInstanceOf('Titon\View\Helper', $this->object->getHelper('html'));
        $this->assertInstanceOf('Titon\View\Helper\HtmlHelper', $this->object->getHelper('html'));
    }

    /**
     * @expectedException \Titon\View\Exception\MissingHelperException
     */
    public function testGetHelperMissingThrowsException(): void {
        $this->object->getHelper('foobar');
    }

    public function testGetSetVariables(): void {
        $expected = Map {};
        $this->assertEquals($expected, $this->object->getVariables());

        $this->object->setVariable('foo', 'bar');
        $expected['foo'] = 'bar';

        $this->assertEquals($expected, $this->object->getVariables());

        $this->object->setVariables(Map {
            'numeric' => 1337,
            'boolean' => false
        });
        $expected['numeric'] = 1337;
        $expected['boolean'] = false;

        $this->assertEquals($expected, $this->object->getVariables());

        $this->assertEquals('bar', $this->object->getVariable('foo'));
    }

    public function testSetHelperSetsVariable(): void {
        $this->assertEquals(null, $this->object->getVariable('html'));

        $helper = new HtmlHelper();
        $this->object->setHelper('html', $helper);

        $this->assertEquals($helper, $this->object->getVariable('html'));
    }

    public function testSetHelperSetsRegistry(): void {
        $this->assertFalse(Registry::has('Titon\View\Helper\HtmlHelper'));

        $this->object->setHelper('html', new HtmlHelper());

        $this->assertTrue(Registry::has('Titon\View\Helper\HtmlHelper'));
    }

    public function testSetVariableInflectsName(): void {
        $this->object->setVariable('123', 'abc');

        $this->assertEquals(null, $this->object->getVariable('123'));
        $this->assertEquals('abc', $this->object->getVariable('_123'));

        $this->object->setVariable('foo bar', 'abc');

        $this->assertEquals(null, $this->object->getVariable('foo bar'));
        $this->assertEquals('abc', $this->object->getVariable('foobar'));
    }

}
