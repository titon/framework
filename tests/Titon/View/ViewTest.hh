<?hh
namespace Titon\View;

use Titon\Utility\Config;
use Titon\Test\Stub\View\ViewStub;
use Titon\Test\TestCase;
use Titon\View\Helper\HtmlHelper;
use Titon\View\Helper\FormHelper;
use Titon\View\Locator\TemplateLocator;

/**
 * @property \Titon\View\View $object
 */
class ViewTest extends TestCase {

    protected function setUp(): void {
        parent::setUp();

        $this->vfs()->createStructure(static::generateViewStructure());

        Config::set('titon.path.views', Vector {$this->vfs()->path('/views/inherited/')});

        $this->object = new ViewStub(new TemplateLocator([
            $this->vfs()->path('/views'),
            $this->vfs()->path('/views/fallback')
        ]));
    }

    public static function generateViewStructure(): array<string, mixed> {
        return [
            '/views/' => [
                'fallback/' => [
                    'private/' => [
                        'emails/' => [
                            'example.html.tpl' => "<!DOCTYPE html>\n<html>\n<body>\n    This is an example email template.<br>\n    It is an <b>HTML</b> specific <i>template</i>.<br>\n    - Titon\n</body>\n</html>",
                        ],
                        'layouts/' => [
                            'fallback.tpl' => '<fallbackLayout><?php echo $this->getContent(); ?></fallbackLayout>'
                        ],
                        'wrappers/' => [
                            'fallback.tpl' => '<fallbackWrapper><?php echo $this->getContent(); ?></fallbackWrapper>'
                        ]
                    ]
                ],
                'private/' => [
                    'errors/' => [
                        '404.tpl' => '404.tpl'
                    ],
                    'layouts/' => [
                        'default.tpl' => '<layout><?php echo $this->getContent(); ?></layout>'
                    ],
                    'partials/' => [
                        'nested/' => [
                            'include.tpl' => 'nested/include.tpl'
                        ],
                        'include.tpl' => 'include.tpl',
                        'variables.tpl' => '<?php echo $name; ?> - <?php echo $type; ?> - <?php echo $filename; ?>'
                    ],
                    'wrappers/' => [
                        'wrapper.tpl' => '<wrapper><?php echo $this->getContent(); ?></wrapper>'
                    ],
                    'root.tpl' => 'private/root.tpl'
                ],
                'public/' => [
                    'index/' => [
                        'add.tpl' => 'add.tpl',
                        'edit.tpl' => 'edit.tpl',
                        'index.tpl' => 'index.tpl',
                        'test-include.tpl' => 'test-include.tpl <?php echo $this->open(\'nested/include\'); ?>',
                        'view.tpl' => 'view.tpl',
                        'view.xml.tpl' => 'view.xml.tpl',
                    ],
                    'lang/' => [
                        'index.en-us.tpl' => 'index.en-us.tpl',
                        'index.fr.tpl' => 'index.fr.tpl',
                        'index.tpl' => 'index.tpl',
                    ],
                    'root.tpl' => 'public/root.tpl'
                ]
            ]
        ];
    }

/*
    public function testPaths(): void {
        $expected = Vector {
            $this->vfs()->path('/views/'),
            $this->vfs()->path('/views/fallback/'),
            $this->vfs()->path('/views/inherited/')
        };
        $this->assertEquals($expected, $this->object->getPaths());

        $this->object->addPath(TEST_DIR);
        $expected[] = str_replace('\\', '/', TEST_DIR) . '/';
        $this->assertEquals($expected, $this->object->getPaths());
    } */

    public function testHelpers(): void {
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
    }

    /**
     * @expectedException \Titon\View\Exception\MissingHelperException
     */
    public function testHelperMissing(): void {
        $this->object->getHelper('foobar');
    }

    public function testVariables(): void {
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

/*
    public function testLocateTemplateLocales(): void {
        $rootPath = $this->vfs()->path('/views/');

        $view1 = new ViewStub([$rootPath]);
        $view1->setLocales(Vector {});

        $this->assertEquals($rootPath . 'public/lang/index.tpl', $view1->locateTemplate('lang/index'));

        $view2 = new ViewStub([$rootPath]);
        $view2->setLocales(Vector {'en-us'});

        $this->assertEquals($rootPath . 'public/lang/index.en-us.tpl', $view2->locateTemplate('lang/index'));

        $view3 = new ViewStub([$rootPath]);
        $view3->setLocales(Vector {'en', 'fr'});

        $this->assertEquals($rootPath . 'public/lang/index.fr.tpl', $view3->locateTemplate('lang/index'));
    }*/

}
