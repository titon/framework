# Events #

The following events are available in the Intl package.

### intl.detecting ###

The `intl.detecting` event, represented by `Titon\Intl\Event\DetectingEvent`, is triggered before the locale is detected in `detect()`.

<table class="table is-striped">
    <thead>
        <tr>
            <th>Argument</th>
            <th>Type</th>
            <th>Getter</th>
            <th>Setter</th>
            <th>Description</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td>Translator</td>
            <td>Titon\Intl\Translator</td>
            <td>getTranslator()</td>
            <td></td>
            <td>The translator in which the locale is being detected.</td>
        </tr>
    </tbody>
</table>

### intl.detected ###

The `intl.detected` event, represented by `Titon\Intl\Event\DetectedEvent`, is triggered after the locale is detected in `detect()`.

<table class="table is-striped">
    <thead>
        <tr>
            <th>Argument</th>
            <th>Type</th>
            <th>Getter</th>
            <th>Setter</th>
            <th>Description</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td>Translator</td>
            <td>Titon\Intl\Translator</td>
            <td>getTranslator()</td>
            <td></td>
            <td>The translator in which the locale is being detected.</td>
        </tr>
        <tr>
            <td>Locale</td>
            <td>Titon\Intl\Locale</td>
            <td>getLocale()</td>
            <td></td>
            <td>The locale that was detected.</td>
        </tr>
    </tbody>
</table>

### intl.localize ###

The `intl.localize` event, represented by `Titon\Intl\Event\LocalizeEvent`, is triggered when the locale is set with `localize()`. This event is also triggered during `detect()`.

<table class="table is-striped">
    <thead>
        <tr>
            <th>Argument</th>
            <th>Type</th>
            <th>Getter</th>
            <th>Setter</th>
            <th>Description</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td>Translator</td>
            <td>Titon\Intl\Translator</td>
            <td>getTranslator()</td>
            <td></td>
            <td>The translator in which the locale was set.</td>
        </tr>
        <tr>
            <td>Locale</td>
            <td>Titon\Intl\Locale</td>
            <td>getLocale()</td>
            <td>setLocale()</td>
            <td>The locale that was set.</td>
        </tr>
    </tbody>
</table>


