<?hh
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

require '../vendor/autoload.php';

use Titon\Console\Application;
use Titon\Console\Console;
use Titon\Console\Input;
use Titon\Console\Output;
use Titon\Kernel\Middleware\Pipeline;
use Titon\Test\Stub\Console\CommandStub;

$banner = <<<'BANNER'
 ______  ______   ______   __    __   ______   __    __   ______   ______   __  __
/\  ___\/\  __ \ /\  __ \ /\ '-./  \ /\  ___\ /\ \ __\ \ /\  __ \ /\  __ \ /\ \/ /_
\ \  __\\ \  __< \ \  __ \\ \ \._/\ \\ \  __\ \ \ \| \\ \\ \ \/\ \\ \  __< \ \  _  \
 \ \_\/  \ \_\ \_\\ \_\ \_\\ \_\/\ \_\\ \_____\\ \__/_.__\\ \_____\\ \_\ \_\\ \_\ \_\
  \/_/    \/_/\/_/ \/_/\/_/ \/_/  \/_/ \/_____/ \/_/  \/_/ \/_____/ \/_/\/_/ \/_/\/_/

BANNER;

$application = new Application();
$application->setName('Titon Framework');
$application->setVersion(file_get_contents('../version.md'));
$application->setBanner($banner);
$application->addCommand(new CommandStub());

$console = new Console($application, new Pipeline());
$console->run(new Input(), new Output());
$console->terminate();
