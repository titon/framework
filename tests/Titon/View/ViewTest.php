<?php
namespace Titon\View;

use Titon\Utility\Config;
use Titon\View\View;
use Titon\View\Helper\Html\HtmlHelper;
use Titon\View\Helper\Html\FormHelper;
use Titon\Test\TestCase;
use Titon\View\View\AbstractView;
use VirtualFileSystem\FileSystem;

/**
 * @property \Titon\View\View $object
 * @property \VirtualFileSystem\FileSystem $vfs
 */
class ViewTest extends TestCase {

    protected function setUp() {
        parent::setUp();

        $this->vfs = new FileSystem();
        $this->vfs->createStructure([
            '/views/' => [
                'css/' => [
                    'test.css' => ''
                ],
                'fallback/' => [
                    'private/' => [
                        'emails/' => [
                            'example.html.tpl' => "<!DOCTYPE html>\n<html>\n<body>\n    This is an example email template.<br>\n    It is an <b>HTML</b> specific <i>template</i>.<br>\n    - Titon\n</body>\n</html>",
                            'example.tpl' => "This is an example email template.\nIt is also a plain text email.\n- Titon"
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
                        '404.tpl' => '404.tpl',
                        'error.tpl' => '<?php echo $message; ?>'
                    ],
                    'layouts/' => [
                        'blank.tpl' => '<?php echo $this->getContent(); ?>',
                        'blank.xml.tpl' => '<?php echo $this->getContent(); ?>',
                        'default.tpl' => '<layout><?php echo $this->getContent(); ?></layout>',
                        'error.tpl' => '<error><?php echo $this->getContent(); ?></error>'
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
                        'test-include.tpl' => 'test-include.tpl <?php echo $this->open(\nested/include\'); ?>',
                        'view.tpl' => 'view.tpl',
                        'view.xml.tpl' => 'view.xml.tpl',
                    ],
                    'root.tpl' => 'public/root.tpl'
                ]
            ]
        ]);

        $this->object = new ViewStub([
            $this->vfs->path('/views/'),
            $this->vfs->path('/views/fallback/')
        ]);
    }

    public function testPaths() {
        $expected = [
            $this->vfs->path('/views/'),
            $this->vfs->path('/views/fallback/')
        ];
        $this->assertEquals($expected, $this->object->getPaths());

        $this->object->addPath(TEST_DIR);
        $expected[] = TEST_DIR;
        $this->assertEquals($expected, $this->object->getPaths());
    }

    public function testPathsNotDefined() {
        $view = new ViewStub();
        $view->locateTemplate('index/add');
    }

    public function testHelpers() {
        $form = new FormHelper();
        $html = new HtmlHelper();

        $this->assertEquals([], $this->object->getHelpers());

        $this->object->addHelper('html', $html);
        $this->assertEquals([
            'html' => $html
        ], $this->object->getHelpers());

        $this->object->addHelper('form', $form);
        $this->assertEquals([
            'html' => $html,
            'form' => $form
        ], $this->object->getHelpers());

        $this->assertInstanceOf('Titon\View\Helper', $this->object->getHelper('html'));
    }

    public function testHelperMissing() {
        $this->object->getHelper('foobar');
    }

    public function testVariables() {
        $expected = [];
        $this->assertEquals($expected, $this->object->getVariables());

        $this->object->setVariable('foo', 'bar');
        $expected['foo'] = 'bar';
        $this->assertEquals($expected, $this->object->getVariables());

        $this->object->setVariables([
            'numeric' => 1337,
            'boolean' => false
        ]);
        $expected['numeric'] = 1337;
        $expected['boolean'] = false;
        $this->assertEquals($expected, $this->object->getVariables());

        $this->assertEquals('bar', $this->object->getVariable('foo'));
    }

    public function testLocateTemplate() {
        $this->assertEquals($this->vfs->path('/views/public/index/add.tpl'), $this->object->locateTemplate(['index', 'add']));
        $this->assertEquals($this->vfs->path('/views/public/index/add.tpl'), $this->object->locateTemplate('index/add'));
        $this->assertEquals($this->vfs->path('/views/public/index/add.tpl'), $this->object->locateTemplate('index\add'));
        $this->assertEquals($this->vfs->path('/views/public/index/view.xml.tpl'), $this->object->locateTemplate(['controller' => 'index', 'action' => 'view', 'ext' => 'xml']));

        // partials
        $this->assertEquals($this->vfs->path('/views/private/partials/include.tpl'), $this->object->locateTemplate('include', View::PARTIAL));
        $this->assertEquals($this->vfs->path('/views/private/partials/nested/include.tpl'), $this->object->locateTemplate('nested/include', View::PARTIAL));

        // wrapper
        $this->assertEquals($this->vfs->path('/views/private/wrappers/wrapper.tpl'), $this->object->locateTemplate('wrapper', View::WRAPPER));
        $this->assertEquals($this->vfs->path('/views/fallback/private/wrappers/fallback.tpl'), $this->object->locateTemplate('fallback', View::WRAPPER));

        // layout
        $this->assertEquals($this->vfs->path('/views/private/layouts/default.tpl'), $this->object->locateTemplate('default', View::LAYOUT));
        $this->assertEquals($this->vfs->path('/views/fallback/private/layouts/fallback.tpl'), $this->object->locateTemplate('fallback', View::LAYOUT));

        // private
        $this->assertEquals($this->vfs->path('/views/private/errors/404.tpl'), $this->object->locateTemplate('errors/404', View::PRIVATE_TEMPLATE));
        $this->assertEquals($this->vfs->path('/views/fallback/private/emails/example.html.tpl'), $this->object->locateTemplate(['emails', 'example', 'ext' => 'html'], View::PRIVATE_TEMPLATE));

        // private (default)
        $this->assertEquals($this->vfs->path('/views/private/errors/404.tpl'), $this->object->locateTemplate('errors/404', 'INVALID'));
        $this->assertEquals($this->vfs->path('/views/fallback/private/emails/example.html.tpl'), $this->object->locateTemplate(['emails', 'example', 'ext' => 'html'], 'INVALID'));
    }

    public function testLocateTemplateMissing() {
        $this->object->locateTemplate(['index', 'missing']);
    }

    public function testLocateTemplateLocales() {
        $testTemplates = [];

        Config::set('titon.path.views', [$this->vfs->path('/views/locales/')]);
        Config::set('titon.locale.cascade', []);

        $this->object->on('view.postLocate', function($event, $templates, $type, $paths) use (&$testTemplates) {
            $testTemplates = $templates;
        });

        $this->object->locateTemplate('index/add');

        $this->assertEquals([
            '/public/index/add.tpl'
        ], $testTemplates);

        Config::set('titon.locale.cascade', ['en']);

        $this->object->flushCache();
        $this->object->locateTemplate('index/add');

        $this->assertEquals([
            '/public/index/add.en.tpl',
            '/public/index/add.tpl'
        ], $testTemplates);

        Config::set('titon.locale.cascade', ['en-us', 'en', 'fr']);

        $this->object->flushCache();
        $this->object->locateTemplate('index/add');

        $this->assertEquals([
            '/public/index/add.en-us.tpl',
            '/public/index/add.en.tpl',
            '/public/index/add.fr.tpl',
            '/public/index/add.tpl'
        ], $testTemplates);
    }

}

class ViewStub extends AbstractView {

    public function render($template, $private = false) {}
    public function renderTemplate($path, array $variables = array()) {}

}