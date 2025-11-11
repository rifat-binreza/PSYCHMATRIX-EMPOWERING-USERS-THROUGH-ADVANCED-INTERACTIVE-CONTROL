import React from 'react';
import { Text, TextProps, TextStyle } from 'react-native';

type ThemedTextProps = {
  type?: 'title' | 'link' | 'body';
  style?: TextStyle | TextStyle[];
} & TextProps;

export const ThemedText: React.FC<ThemedTextProps> = ({ type = 'body', style, children, ...rest }) => {
  const sizeMap = { title: 22, link: 16, body: 14 } as any;
  const colorMap = { title: '#000', link: '#1e90ff', body: '#000' } as any;
  return (
    <Text style={[{ fontSize: sizeMap[type], color: colorMap[type] }, style]} {...rest}>
      {children}
    </Text>
  );
};

export default ThemedText;
