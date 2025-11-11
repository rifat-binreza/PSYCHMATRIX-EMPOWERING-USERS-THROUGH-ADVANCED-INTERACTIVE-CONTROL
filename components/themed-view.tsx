import React from 'react';
import { View, ViewProps, ViewStyle } from 'react-native';

type Props = { style?: ViewStyle | ViewStyle[] } & ViewProps;

export const ThemedView: React.FC<Props> = ({ style, children, ...rest }) => {
  return (
    <View style={[{ flex: 1, padding: 16, backgroundColor: 'transparent' }, style]} {...rest}>
      {children}
    </View>
  );
};

export default ThemedView;
