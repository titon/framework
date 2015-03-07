<?hh
namespace Titon\View;

use Titon\Common\DataMap;
use Titon\Test\Stub\View\ViewStub;
use Titon\Utility\Config;
use Titon\View\Helper\HtmlHelper;
use Titon\View\Helper\FormHelper;
use Titon\Test\TestCase;

/**
 * @property \Titon\View\View $object
 */
class ViewTest extends TestCase {

    protected function setUp(): void {
        parent::setUp();

        $this->vfs()->createStructure([
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
                        'include.tpl' => 'include.tpl'
                    ],
                    'wrappers/' => [
                        'wrapper.tpl' => '<wrapper><?php echo $this->getContent(); ?></wrapper>'
                    ]
                ],
                'public/' => [
                    'index/' => [
                        'add.tpl' => 'add.tpl',
                        'view.xml.tpl' => 'view.xml.tpl',
                    ],
                    'lang/' => [
                        'index.en-us.tpl' => 'index.en-us.tpl',
                        'index.fr.tpl' => 'index.fr.tpl',
                        'index.tpl' => 'index.tpl',
                    ]
                ]
            ]
        ]);

        Config::set('titon.path.views', Vector {$this->vfs()->path('/views/inherited/')});

        $this->object = new ViewStub([
            $this->vfs()->path('/views'),
            $this->vfs()->path('/views/fallback')
        ]);
    }

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
    }

    public function testHelpers(): void {
        $form = new FormHelper();
        $html = new HtmlHelper();

        $this->assertEquals(Map {}, $this->object->getHelpers());

        $this->object->addHelper('html', $html);
        $this->assertEquals(Map {
            'html' => $html
        }, $this->object->getHelpers());

        $this->object->addHelper('form', $form);
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

    public function testLocateTemplate(): void {
        $this->assertEquals($this->vfs()->path('/views/public/index/add.tpl'), $this->object->locateTemplate('index/add'));
        $this->assertEquals($this->vfs()->path('/views/public/index/add.tpl'), $this->object->locateTemplate('index\add'));
        $this->assertEquals($this->vfs()->path('/views/public/index/add.tpl'), $this->object->locateTemplate('index/add.tpl'));
        $this->assertEquals($this->vfs()->path('/views/public/index/view.xml.tpl'), $this->object->locateTemplate('index/view.xml'));

        // partials
        $this->assertEquals($this->vfs()->path('/views/private/partials/include.tpl'), $this->object->locateTemplate('include', Template::PARTIAL));
        $this->assertEquals($this->vfs()->path('/views/private/partials/nested/include.tpl'), $this->object->locateTemplate('nested/include', Template::PARTIAL));

        // wrapper
        $this->assertEquals($this->vfs()->path('/views/private/wrappers/wrapper.tpl'), $this->object->locateTemplate('wrapper', Template::WRAPPER));
        $this->assertEquals($this->vfs()->path('/views/fallback/private/wrappers/fallback.tpl'), $this->object->locateTemplate('fallback', Template::WRAPPER));

        // layout
        $this->assertEquals($this->vfs()->path('/views/private/layouts/default.tpl'), $this->object->locateTemplate('default', Template::LAYOUT));
        $this->assertEquals($this->vfs()->path('/views/fallback/private/layouts/fallback.tpl'), $this->object->locateTemplate('fallback', Template::LAYOUT));

        // private
        $this->assertEquals($this->vfs()->path('/views/private/errors/404.tpl'), $this->object->locateTemplate('errors/404', Template::CLOSED));
        $this->assertEquals($this->vfs()->path('/views/fallback/private/emails/example.html.tpl'), $this->object->locateTemplate('emails/example.html', Template::CLOSED));
    }

    /**
     * @expectedException \Titon\View\Exception\MissingTemplateException
     */
    public function testLocateTemplateMissing(): void {
        $this->object->locateTemplate('index/missing');
    }

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
    }

}
