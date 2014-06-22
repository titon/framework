<?php
namespace Titon\Environment;

use Titon\Test\TestCase;
use Titon\Utility\Path;

/**
 * @property \Titon\Environment\EnvironmentStub $object
 */
class EnvironmentTest extends TestCase {

    protected function setUp() {
        parent::setUp();

        $this->object = new EnvironmentStub();

        $host = new Host(['dev', '123.0.0.0']);
        $host->setBootstrap(TEMP_DIR . '/environment/dev.php');

        $this->object->addHost('dev', $host);

        $host = new Host(['prod', '123.456.0.0'], Environment::PRODUCTION);
        $host->setBootstrap(TEMP_DIR . '/environment/prod.php');

        $this->object->addHost('prod', $host);
        $this->object->addHost('staging', new Host(['staging', '123.456.789.0'], Environment::STAGING));
    }

    public function testCurrent() {
        // dev
        $_SERVER['HTTP_HOST'] = 'dev';
        $this->object->initialize();
        $this->assertInstanceOf('Titon\Environment\Host', $this->object->current());
        $this->assertEquals('dev', $this->object->current()->getKey());
        $this->assertEquals(Environment::DEVELOPMENT, $this->object->current()->getType());

        // dev ip
        $_SERVER['HTTP_HOST'] = '123.0.0.0';
        $this->object->initialize();
        $this->assertEquals('dev', $this->object->current()->getKey());

        // prod
        $_SERVER['HTTP_HOST'] = 'prod';
        $this->object->initialize();
        $this->assertEquals('prod', $this->object->current()->getKey());
        $this->assertEquals(Environment::PRODUCTION, $this->object->current()->getType());

        // prod ip
        $_SERVER['HTTP_HOST'] = '123.456.0.0';
        $this->object->initialize();
        $this->assertEquals('prod', $this->object->current()->getKey());

        // staging
        $_SERVER['HTTP_HOST'] = 'staging';
        $this->object->initialize();
        $this->assertEquals('staging', $this->object->current()->getKey());
        $this->assertEquals(Environment::STAGING, $this->object->current()->getType());

        // staging ip
        $_SERVER['HTTP_HOST'] = '123.456.789.0';
        $this->object->initialize();
        $this->assertEquals('staging', $this->object->current()->getKey());

        // test SERVER_ADDR over HTTP_HOST
        $_SERVER['HTTP_HOST'] = '';
        $_SERVER['SERVER_ADDR'] = '123.0.0.0';
        $this->object->initialize();
        $this->assertEquals('dev', $this->object->current()->getKey());
    }

    public function testSetFallback() {
        $this->object->setFallback('dev');

        $_SERVER['HTTP_HOST'] = 'fake_environment';
        $_SERVER['SERVER_ADDR'] = '0.0.0.0';

        $this->object->initialize();
        $this->assertEquals('dev', $this->object->current()->getKey());
        $this->assertEquals('dev', $this->object->getFallback()->getKey());
    }

    /**
     * @expectedException \Titon\Environment\Exception\MissingHostException
     */
    public function testSetFallbackMissingHost() {
        $this->object->setFallback('fakeEnv');
    }

    public function testGetHosts() {
        $this->assertEquals(3, count($this->object->getHosts()));
    }

    public function testInitialize() {
        $_SERVER['HTTP_HOST'] = 'dev';
        $this->object->initialize();
        $this->assertEquals('dev', $_SERVER['ENV_TEST']);

        $_SERVER['HTTP_HOST'] = 'prod';
        $this->object->initialize();
        $this->assertEquals('prod', $_SERVER['ENV_TEST']);

        // Should be falsey values since staging doesn't exist
        $_SERVER['HTTP_HOST'] = 'staging';
        $_SERVER['ENV_TEST'] = null;
        $this->object->initialize();
        $this->assertEquals(null, $_SERVER['ENV_TEST']);
    }

    public function testInitializeNoHosts() {
        $env = new EnvironmentStub();
        $env->initialize();

        $this->assertEquals(null, $env->current());
    }

    public function testIs() {
        // dev
        $_SERVER['HTTP_HOST'] = 'dev';
        $this->object->initialize();
        $this->assertTrue($this->object->is('dev'));
        $this->assertFalse($this->object->is('prod'));

        // dev ip
        $_SERVER['HTTP_HOST'] = '123.0.0.0';
        $this->object->initialize();
        $this->assertTrue($this->object->is('dev'));
        $this->assertFalse($this->object->is('prod'));

        // prod
        $_SERVER['HTTP_HOST'] = 'prod';
        $this->object->initialize();
        $this->assertTrue($this->object->is('prod'));
        $this->assertFalse($this->object->is('staging'));

        // prod ip
        $_SERVER['HTTP_HOST'] = '123.456.0.0';
        $this->object->initialize();
        $this->assertTrue($this->object->is('prod'));
        $this->assertFalse($this->object->is('staging'));

        // staging
        $_SERVER['HTTP_HOST'] = 'staging';
        $this->object->initialize();
        $this->assertTrue($this->object->is('staging'));
        $this->assertFalse($this->object->is('dev'));

        // staging ip
        $_SERVER['HTTP_HOST'] = '123.456.789.0';
        $this->object->initialize();
        $this->assertTrue($this->object->is('staging'));
        $this->assertFalse($this->object->is('dev'));

        // test SERVER_ADDR over HTTP_HOST
        $_SERVER['HTTP_HOST'] = '';
        $_SERVER['SERVER_ADDR'] = '123.0.0.0';
        $this->object->initialize();
        $this->assertTrue($this->object->is('dev'));
        $this->assertFalse($this->object->is('staging'));
    }

    public function testIsMachine() {
        if (getenv('TRAVIS')) {
            $this->markTestSkipped('Can\'t test hostnames within travis as they are too different per box');
        }

        $env = new Environment();

        // My local computer :D
        // TODO Figure out how to make this work for anyone
        $this->assertTrue($env->isMachine('miles-pc'));
        $this->assertTrue($env->isMachine('miles-*'));
    }

    public function testIsLocalhost() {
        $_SERVER['HTTP_HOST'] = 'domain.com';
        $_SERVER['REMOTE_ADDR'] = '127.33.123.54';
        $this->assertFalse($this->object->isLocalhost());

        $_SERVER['REMOTE_ADDR'] = '127.0.0.1';
        $this->assertTrue($this->object->isLocalhost());

        $_SERVER['REMOTE_ADDR'] = '::1';
        $this->assertTrue($this->object->isLocalhost());

        $_SERVER['HTTP_HOST'] = 'localhost';
        $_SERVER['REMOTE_ADDR'] = '127.33.123.54';
        $this->assertTrue($this->object->isLocalhost());
    }

    public function testIsDevelopment() {
        $_SERVER['HTTP_HOST'] = 'dev';
        $this->object->initialize();
        $this->assertTrue($this->object->isDevelopment());
        $this->assertFalse($this->object->isProduction());
        $this->assertFalse($this->object->isQA());
        $this->assertFalse($this->object->isStaging());
    }

    public function testIsProduction() {
        $_SERVER['HTTP_HOST'] = 'prod';
        $this->object->initialize();
        $this->assertFalse($this->object->isDevelopment());
        $this->assertTrue($this->object->isProduction());
        $this->assertFalse($this->object->isQA());
        $this->assertFalse($this->object->isStaging());
    }

    public function testIsQA() {
        $this->object->addHost('qa', new Host(['qa', '123.456.0.666'], Environment::QA));

        $_SERVER['HTTP_HOST'] = 'qa';
        $this->object->initialize();
        $this->assertFalse($this->object->isDevelopment());
        $this->assertFalse($this->object->isProduction());
        $this->assertTrue($this->object->isQA());
        $this->assertFalse($this->object->isStaging());
    }

    public function testIsStaging() {
        $_SERVER['HTTP_HOST'] = 'staging';
        $this->object->initialize();
        $this->assertFalse($this->object->isDevelopment());
        $this->assertFalse($this->object->isProduction());
        $this->assertFalse($this->object->isQA());
        $this->assertTrue($this->object->isStaging());
    }

    /**
     * @expectedException \Titon\Environment\Exception\MissingBootstrapException
     */
    public function testBootstrapErrorsMissingFile() {
        $env = new Environment(['bootstrapPath' => TEMP_DIR, 'throwMissingError' => true]);
        $env->addHost('dev-us', new Host(['dev', '123.0.0.0']));
        $env->initialize();
    }

    public function testBootstrapDoesntErrorMissingFile() {
        $env = new Environment(['bootstrapPath' => TEMP_DIR, 'throwMissingError' => false]);
        $env->addHost('dev-us', new Host(['dev', '123.0.0.0']));
        $env->initialize();
    }

    public function testAutoBootstrapping() {
        $env = new Environment(['bootstrapPath' => TEMP_DIR]);
        $env->addHost('dev-us', new Host(['dev', '123.0.0.0']));

        $this->assertEquals(Path::ds(TEMP_DIR, true) . 'dev-us.php', $env->getHost('dev-us')->getBootstrap());
    }

}

class EnvironmentStub extends Environment {

    // Use host/IP for testing
    public function isMachine($name) {
        $host = null;

        if (!empty($_SERVER['HTTP_HOST'])) {
            $host = $_SERVER['HTTP_HOST'];
        } else if (!empty($_SERVER['SERVER_ADDR'])) {
            $host = $_SERVER['SERVER_ADDR'];
        }

        return (bool) preg_match('/^' . preg_quote($name, '/') . '/i', $host);
    }

}