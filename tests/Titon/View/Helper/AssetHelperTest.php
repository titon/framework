<?hh
namespace Titon\View\Helper;

use Titon\Utility\Config;
use Titon\Test\TestCase;
use VirtualFileSystem\FileSystem;

class AssetHelperTest extends TestCase {

    public function testScripts() {
        $helper = new AssetHelper();
        $helper
            ->addScript('script.js')
            ->addScript('path/commons.js', AssetHelper::HEADER)
            ->addScript('path/no-extension')
            ->addScript('/a/really/really/deep/path/include.js', AssetHelper::HEADER);

        $this->assertEquals('', $helper->scripts('fakeLocation'));

        $this->assertEquals(
            '<script src="path/commons.js" type="text/javascript"></script>' . PHP_EOL .
            '<script src="/a/really/really/deep/path/include.js" type="text/javascript"></script>' . PHP_EOL
        , $helper->scripts(AssetHelper::HEADER));

        $this->assertEquals(
            '<script src="script.js" type="text/javascript"></script>' . PHP_EOL .
            '<script src="path/no-extension.js" type="text/javascript"></script>' . PHP_EOL
        , $helper->scripts(AssetHelper::FOOTER));

        // with ordering
        $helper = new AssetHelper();
        $helper
            ->addScript('script.js', AssetHelper::FOOTER, 3)
            ->addScript('path/commons.js', AssetHelper::FOOTER, 2)
            ->addScript('path/no-extension', AssetHelper::FOOTER)
            ->addScript('/a/really/really/deep/path/include.js', AssetHelper::FOOTER, 5);

        $this->assertEquals(
            '<script src="path/commons.js" type="text/javascript"></script>' . PHP_EOL .
            '<script src="script.js" type="text/javascript"></script>' . PHP_EOL .
            '<script src="path/no-extension.js" type="text/javascript"></script>' . PHP_EOL .
            '<script src="/a/really/really/deep/path/include.js" type="text/javascript"></script>' . PHP_EOL
        , $helper->scripts(AssetHelper::FOOTER));

        // environment
        $helper = new AssetHelper();
        $helper
            ->addScript('script.js', AssetHelper::FOOTER, 30, 'dev')
            ->addScript('path/commons.js', AssetHelper::FOOTER, 30, 'prod')
            ->addScript('path/no-extension', AssetHelper::FOOTER, 30, 'staging')
            ->addScript('/a/really/really/deep/path/include.js', AssetHelper::FOOTER, 30, 'dev');

        $this->assertEquals(
            '<script src="script.js" type="text/javascript"></script>' . PHP_EOL .
            '<script src="/a/really/really/deep/path/include.js" type="text/javascript"></script>' . PHP_EOL
        , $helper->scripts(AssetHelper::FOOTER, 'dev'));
    }

    public function testStylesheets() {
        $helper = new AssetHelper();
        $helper
            ->addStylesheet('style.css')
            ->addStylesheet('a/really/deep/path/with/no/extension/style.css')
            ->addStylesheet('mobile.css', Map {'media' => 'mobile'});

        $this->assertEquals(
            '<link href="style.css" media="screen" rel="stylesheet" type="text/css">' . PHP_EOL .
            '<link href="a/really/deep/path/with/no/extension/style.css" media="screen" rel="stylesheet" type="text/css">' . PHP_EOL .
            '<link href="mobile.css" media="mobile" rel="stylesheet" type="text/css">' . PHP_EOL
        , $helper->stylesheets());

        // with ordering
        $helper = new AssetHelper();
        $helper
            ->addStylesheet('style.css', Map {'media' => 'handheld'}, 3)
            ->addStylesheet('a/really/deep/path/with/no/extension/style.css', Map {'media' => 'screen'}, 1)
            ->addStylesheet('mobile.css', Map {'media' => 'mobile'}, 2);

        $this->assertEquals(
            '<link href="a/really/deep/path/with/no/extension/style.css" media="screen" rel="stylesheet" type="text/css">' . PHP_EOL .
            '<link href="mobile.css" media="mobile" rel="stylesheet" type="text/css">' . PHP_EOL .
            '<link href="style.css" media="handheld" rel="stylesheet" type="text/css">' . PHP_EOL
        , $helper->stylesheets());

        // environment
        $helper = new AssetHelper();
        $helper
            ->addStylesheet('style.css', Map {'media' => 'handheld'}, 30, 'dev')
            ->addStylesheet('a/really/deep/path/with/no/extension/style.css', Map {'media' => 'screen'}, 30, 'staging')
            ->addStylesheet('mobile.css', Map {'media' => 'mobile'}, 30, 'prod');

        $this->assertEquals('<link href="style.css" media="handheld" rel="stylesheet" type="text/css">' . PHP_EOL, $helper->stylesheets('dev'));
    }

    public function testPreparePath() {
        $helper = new AssetHelper();

        $this->assertEquals('style.css', $helper->preparePath('style', 'css'));
        $this->assertEquals('style.css', $helper->preparePath('style.css', 'css'));
        $this->assertEquals('/css/style.css', $helper->preparePath('/css/style', 'css'));
        $this->assertEquals('/css/style.min.css', $helper->preparePath('/css/style.min', 'css'));
        $this->assertEquals('/css/style.min.css?v=1', $helper->preparePath('/css/style.min?v=1', 'css'));

        // Should not change
        $this->assertEquals('//domain.com/style', $helper->preparePath('//domain.com/style', 'css'));
        $this->assertEquals('http://domain.com/script', $helper->preparePath('http://domain.com/script', 'js'));
        $this->assertEquals('https://domain.com/asset?version=123', $helper->preparePath('https://domain.com/asset?version=123', 'js'));
    }

    public function testTimestamping() {
        $vfs = new FileSystem();
        $vfs->createDirectory('/css/');
        $vfs->createFile('/css/test.css');

        $helper = new AssetHelper(Map {'timestamp' => true, 'webroot' => $vfs->path('/')});
        $helper->addStylesheet('/css/test.css');

        $this->assertRegExp('/<link href="\/css\/test\.css\?([0-9]+)" media="screen" rel="stylesheet" type="text\/css">/', $helper->stylesheets());

        unset($vfs);
    }

    public function testTimestampingNoWebroot() {
        $helper = new AssetHelper(Map {'timestamp' => true});
        $helper->addStylesheet('/css/test.css');

        $this->assertEquals('<link href="/css/test.css" media="screen" rel="stylesheet" type="text/css">' . PHP_EOL, $helper->stylesheets());
    }

    public function testTimestampingThroughConfig() {
        $vfs = new FileSystem();
        $vfs->createDirectory('/css/');
        $vfs->createFile('/css/test.css');

        Config::set('titon.webroot', $vfs->path('/'));

        $helper = new AssetHelper(Map {'timestamp' => true, 'webroot' => null});
        $helper->addStylesheet('/css/test.css');

        $this->assertRegExp('/<link href="\/css\/test\.css\?([0-9]+)" media="screen" rel="stylesheet" type="text\/css">/', $helper->stylesheets());

        unset($vfs);
    }

}